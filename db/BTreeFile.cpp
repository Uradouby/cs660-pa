#include <db/BTreeFile.h>
#include <db/Database.h>
#include <cassert>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace db;

BTreeLeafPage *BTreeFile::findLeafPage(TransactionId tid, PagesMap &dirtypages, BTreePageId *pid, Permissions perm,
                                       const Field *f) {
    if (pid->getType() == BTreePageType::LEAF) {
        return dynamic_cast<BTreeLeafPage *>(getPage(tid, dirtypages, pid, perm));
    }
    if (pid->getType() != BTreePageType::INTERNAL) {
        throw std::runtime_error("Invalid page");
    }
    auto *page = dynamic_cast<BTreeInternalPage *>(getPage(tid, dirtypages, pid, perm));
    auto it = page->begin();
    auto end = page->end();
    if (!(it != end)) {
        throw std::runtime_error("Empty page");
    }
    BTreeEntry *entry;
    do {
        entry = &*it;
        if (f == nullptr || f->compare(Op::LESS_THAN_OR_EQ, entry->getKey())) {
            return findLeafPage(tid, dirtypages, entry->getLeftChild(), perm, f);
        }
    } while (it != end);
    return findLeafPage(tid, dirtypages, entry->getRightChild(), perm, f);
}

BTreeLeafPage *BTreeFile::splitLeafPage(TransactionId tid, PagesMap &dirtypages, BTreeLeafPage *page, const Field *field) {
    auto *rightPage = dynamic_cast<BTreeLeafPage *>(getEmptyPage(tid, dirtypages, BTreePageType::LEAF));
    BTreePageId *nextPageId = page->getRightSiblingId();
    if (nextPageId != nullptr) {
        auto next = dynamic_cast<BTreeLeafPage *>(getPage(tid, dirtypages, nextPageId, Permissions::READ_WRITE));
        next->setLeftSiblingId(const_cast<BTreePageId *>(&rightPage->getId()));
    }
    rightPage->setRightSiblingId(nextPageId);
    rightPage->setLeftSiblingId(const_cast<BTreePageId *>(&page->getId()));
    page->setRightSiblingId(const_cast<BTreePageId *>(&rightPage->getId()));

    int middleIndex = (page->getNumTuples() + 1) / 2 - 1;
    auto it = page->rbegin();
    for (int i = 0; i < middleIndex; i++) {
        Tuple *tuple = &*it;
        page->deleteTuple(tuple);
        rightPage->insertTuple(tuple);
        ++it;
    }

    Tuple *tuple = &*it;
    auto *index = const_cast<Field *>(&tuple->getField(keyField));
    page->deleteTuple(tuple);
    rightPage->insertTuple(tuple);

    BTreeInternalPage *parentPage = getParentWithEmptySlots(tid, dirtypages, page->getParentId(), index);
    parentPage->insertEntry(*new BTreeEntry(index, (BTreePageId *) &page->getId(), (BTreePageId *) &rightPage->getId()));
    updateParentPointers(tid, dirtypages, parentPage);

    return field->compare(Op::LESS_THAN, index) ? page : rightPage;
}

BTreeInternalPage *BTreeFile::splitInternalPage(TransactionId tid, PagesMap &dirtypages, BTreeInternalPage *page,
                                                Field *field) {
    auto *rightPage = dynamic_cast<BTreeInternalPage *>(getEmptyPage(tid, dirtypages, BTreePageType::INTERNAL));

    int middleIndex = (page->getNumEntries() + 1) / 2 - 1;
    auto it = page->rbegin();
    for (int i = 0; i < middleIndex; i++) {
        BTreeEntry *entry = &*it;
        page->deleteKeyAndRightChild(entry);
        rightPage->insertEntry(*entry);
        ++it;
    }

    BTreeEntry *entry = &*it;
    Field *index = entry->getKey();
    page->deleteKeyAndRightChild(entry);
    updateParentPointers(tid, dirtypages, rightPage);

    BTreeInternalPage *parentPage = getParentWithEmptySlots(tid, dirtypages, page.getParentId(), index);
    parentPage->insertEntry(*new BTreeEntry(index, (BTreePageId *) &page->getId(), (BTreePageId *) &rightPage->getId()));
    updateParentPointers(tid, dirtypages, parentPage);

    return field->compare(Op::LESS_THAN, index) ? page : rightPage;
}

void BTreeFile::stealFromLeafPage(BTreeLeafPage *page, BTreeLeafPage *sibling, BTreeInternalPage *parent,
                                  BTreeEntry *entry, bool isRightSibling) {
    int itemsToMove = (sibling->getNumTuples() - page->getNumTuples()) / 2;
    if (itemsToMove <= 0) {
        throw std::runtime_error("Nothing to steal.");
    }

    auto it = isRightSibling ? sibling->begin() : sibling->rbegin();
    Tuple *tuple;
    do {
        tuple = &*it;
        sibling->deleteTuple(tuple);
        page->insertTuple(tuple);
        ++it;
    } while (--itemsToMove > 0);

    if (isRightSibling) {
        tuple = &*it;
    }

    entry->setKey(const_cast<Field *>(&tuple->getField(keyField)));
    parent->updateEntry(entry);
}

void BTreeFile::stealFromLeftInternalPage(TransactionId tid, PagesMap &dirtypages, BTreeInternalPage *page,
                                          BTreeInternalPage *leftSibling, BTreeInternalPage *parent,
                                          BTreeEntry *parentEntry) {
    int itemsToMove = (leftSibling->getNumEntries() - page->getNumEntries()) / 2;
    if (itemsToMove <= 0) {
        throw std::runtime_error("Nothing to steal.");
    }

    auto it = leftSibling->rbegin();
    auto rightEntry = &*page->begin();

    while (itemsToMove-- > 0) {
        auto leftEntry = &*it;

        parent->deleteKeyAndRightChild(parentEntry);
        parentEntry->setLeftChild(leftEntry->getRightChild());
        parentEntry->setRightChild(rightEntry->getLeftChild());
        page->insertEntry(*parentEntry);

        leftSibling->deleteKeyAndRightChild(leftEntry);
        leftEntry->setLeftChild(const_cast<BTreePageId *>(&leftSibling->getId()));
        leftEntry->setRightChild(const_cast<BTreePageId *>(&page->getId()));
        parent->insertEntry(*leftEntry);

        rightEntry = parentEntry;
        parentEntry = leftEntry;

        ++it;
    }

    updateParentPointers(tid, dirtypages, page);
}

void BTreeFile::stealFromRightInternalPage(TransactionId tid, PagesMap &dirtypages, BTreeInternalPage *page,
                                           BTreeInternalPage *rightSibling, BTreeInternalPage *parent,
                                           BTreeEntry *parentEntry) {
    int itemsToMove = (rightSibling->getNumEntries() - page->getNumEntries()) / 2;
    if (itemsToMove <= 0) {
        throw std::runtime_error("Nothing to steal.");
    }

    auto it = rightSibling->begin();
    auto leftEntry = &*page->rbegin();

    while (itemsToMove-- > 0) {
        auto rightEntry = &*it;

        parent->deleteKeyAndRightChild(parentEntry);
        parentEntry->setLeftChild(leftEntry->getRightChild());
        parentEntry->setRightChild(rightEntry->getLeftChild());
        page->insertEntry(*parentEntry);

        rightSibling->deleteKeyAndLeftChild(rightEntry);
        rightEntry->setLeftChild(const_cast<BTreePageId *>(&page->getId()));
        rightEntry->setRightChild(const_cast<BTreePageId *>(&rightSibling->getId()));
        parent->insertEntry(*rightEntry);

        leftEntry = parentEntry;
        parentEntry = rightEntry;

        ++it;
    }

    updateParentPointers(tid, dirtypages, page);
}

void BTreeFile::mergeLeafPages(TransactionId tid, PagesMap &dirtypages, BTreeLeafPage *leftPage,
                               BTreeLeafPage *rightPage, BTreeInternalPage *parent, BTreeEntry *parentEntry) {
    for (auto &tuple: *rightPage) {
        rightPage->deleteTuple(&tuple);
        leftPage->insertTuple(&tuple);
    }

    auto nextPageId = rightPage->getRightSiblingId();
    leftPage->setRightSiblingId(nextPageId);
    if (nextPageId != nullptr) {
        auto *nextPage = dynamic_cast<BTreeLeafPage *>(getPage(tid, dirtypages, nextPageId, Permissions::READ_WRITE));
        nextPage->setLeftSiblingId(const_cast<BTreePageId *>(&leftPage->getId()));
    }

    deleteParentEntry(tid, dirtypages, leftPage, parent, parentEntry);

    setEmptyPage(tid, dirtypages, rightPage->getId().pageNumber());
}

void BTreeFile::mergeInternalPages(TransactionId tid, PagesMap &dirtypages, BTreeInternalPage *leftPage,
                                   BTreeInternalPage *rightPage, BTreeInternalPage *parent, BTreeEntry *parentEntry) {
    deleteParentEntry(tid, dirtypages, leftPage, parent, parentEntry);

    BTreeEntry *rightEntry = &*rightPage->begin();
    BTreeEntry *leftEntry = &*leftPage->rbegin();

    parentEntry->setLeftChild(leftEntry->getRightChild());
    parentEntry->setRightChild(rightEntry->getLeftChild());
    leftPage->insertEntry(*parentEntry);

    for (auto &entryBTree: *rightPage) {
        rightPage->deleteKeyAndRightChild(&entryBTree);
        leftPage->insertEntry(entryBTree);
    }

    updateParentPointers(tid, dirtypages, leftPage);

    setEmptyPage(tid, dirtypages, rightPage->getId().pageNumber());
}

BTreeFile::BTreeFile(const char *fname, int key, const TupleDesc &td) : keyField(key), td(td) {
    fd = open(fname, O_RDWR | O_CREAT | O_APPEND | O_TRUNC, 0644);
    if (fd == -1) {
        throw std::runtime_error("open");
    }
    tableid = std::hash<std::string>{}(fname);
}