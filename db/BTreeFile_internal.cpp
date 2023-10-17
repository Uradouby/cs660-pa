#include <db/Database.h>
#include <db/BufferPool.h>
#include <db/BTreeFile.h>
#include <db/BTreeHeaderPage.h>
#include <cassert>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace db;

BTreeFileIterator::BTreeFileIterator(TransactionId tid, IndexPredicate *pred, BTreeFile *file, bool done) : tid(tid), pred(pred), file(file) {
    if (done) {
        current_leaf = nullptr;
        return;
    }
    BTreeRootPtrPage *rootPtr = dynamic_cast<BTreeRootPtrPage *>(Database::getBufferPool().getPage(
            BTreeRootPtrPage::getId(file->getId())));
    BTreePageId *root = rootPtr->getRootId();
    if (pred && (pred->getOp() == Op::EQUALS || pred->getOp() == Op::GREATER_THAN || pred->getOp() == Op::GREATER_THAN_OR_EQ)) {
        current_leaf = file->findLeafPage(tid, root, Permissions::READ_ONLY, pred->getField());
    } else {
        current_leaf = file->findLeafPage(tid, root, Permissions::READ_ONLY, nullptr);
    }
    it = current_leaf->begin();
}

BTreeFileIterator BTreeIterable::begin() {
    return {id, pPredicate, pFile};
}

BTreeFileIterator BTreeIterable::end() {
    return {id, pPredicate, pFile, true};
}

bool BTreeFileIterator::operator!=(const BTreeFileIterator &other) const {
    return current_leaf != other.current_leaf;
}

BTreeFileIterator &BTreeFileIterator::operator++() {
    if (it != current_leaf->end()) {
        ++it;
        if (it != current_leaf->end()) {
            return *this;
        }
    }
    auto next_id = current_leaf->getRightSiblingId();
    if (next_id == nullptr) {
        current_leaf = nullptr;
        return *this;
    }
    current_leaf = (BTreeLeafPage*) Database::getBufferPool().getPage(next_id);
    it = current_leaf->begin();
    return *this;
}

Tuple &BTreeFileIterator::operator*() const {
    return *it;
}

void BTreeFile::updateParentPointer(TransactionId tid, PagesMap &dirtypages, const BTreePageId *pid, BTreePageId *child) {
    Page *page = getPage(tid, dirtypages, child, Permissions::READ_ONLY);
    auto p = dynamic_cast<BTreePage *>(page);

    if (*p->getParentId() != *pid) {
        page = getPage(tid, dirtypages, child, Permissions::READ_WRITE);
        p = dynamic_cast<BTreePage *>(page);
        p->setParentId(pid);
    }
}

void BTreeFile::handleMinOccupancyPage(TransactionId tid, PagesMap &dirtypages, BTreePage *page) {
    BTreePageId *parentId = page->getParentId();
    BTreeEntry *leftEntry = nullptr;
    BTreeEntry *rightEntry = nullptr;
    BTreeInternalPage *parent = nullptr;

    // find the left and right siblings through the parent so we make sure they have
    // the same parent as the page. Find the entries in the parent corresponding to
    // the page and siblings
    if(parentId->getType() != BTreePageType::ROOT_PTR) {
        parent = dynamic_cast<BTreeInternalPage *>(getPage(tid, dirtypages, parentId, Permissions::READ_WRITE));
        auto it = parent->begin();
        auto ite = parent->end();
        while(it != ite) {
            BTreeEntry &e = *it;
            if(*e.getLeftChild() == page->getId()) {
                rightEntry = &e;
                break;
            } else if(*e.getRightChild() == page->getId()) {
                leftEntry = &e;
            }
        }
    }

    if(page->getId().getType() == BTreePageType::LEAF) {
        handleMinOccupancyLeafPage(tid, dirtypages, dynamic_cast<BTreeLeafPage *>(page), parent, leftEntry, rightEntry);
    } else { // BTreePageId.INTERNAL
        handleMinOccupancyInternalPage(tid, dirtypages, dynamic_cast<BTreeInternalPage *>(page), parent, leftEntry, rightEntry);
    }
}

void BTreeFile::handleMinOccupancyLeafPage(TransactionId tid, PagesMap &dirtypages, BTreeLeafPage *page,
                                           BTreeInternalPage *parent, BTreeEntry *leftEntry, BTreeEntry *rightEntry) {
    BTreePageId *leftSiblingId = nullptr;
    BTreePageId *rightSiblingId = nullptr;
    if (leftEntry != nullptr) leftSiblingId = leftEntry->getLeftChild();
    if (rightEntry != nullptr) rightSiblingId = rightEntry->getRightChild();

    size_t maxEmptySlots = page->getMaxTuples() - page->getMaxTuples() / 2; // ceiling
    if (leftSiblingId != nullptr) {
        auto *leftSibling = dynamic_cast<BTreeLeafPage *>(getPage(tid, dirtypages, leftSiblingId,
                                                                           Permissions::READ_WRITE));
        // if the left sibling is at minimum occupancy, merge with it. Otherwise, steal some tuples from it
        if (leftSibling->getNumEmptySlots() >= maxEmptySlots) {
            mergeLeafPages(tid, dirtypages, leftSibling, page, parent, leftEntry);
        } else {
            stealFromLeafPage(page, leftSibling, parent, leftEntry, false);
        }
    } else if (rightSiblingId != nullptr) {
        auto *rightSibling = dynamic_cast<BTreeLeafPage *>(getPage(tid, dirtypages, rightSiblingId,
                                                                            Permissions::READ_WRITE));
        // if the right sibling is at minimum occupancy, merge with it. Otherwise, steal some tuples from it
        if (rightSibling->getNumEmptySlots() >= maxEmptySlots) {
            mergeLeafPages(tid, dirtypages, page, rightSibling, parent, rightEntry);
        } else {
            stealFromLeafPage(page, rightSibling, parent, rightEntry, true);
        }
    }
}

void BTreeFile::handleMinOccupancyInternalPage(TransactionId tid, PagesMap &dirtypages, BTreeInternalPage *page,
                                               BTreeInternalPage *parent, BTreeEntry *leftEntry, BTreeEntry *rightEntry) {
    BTreePageId *leftSiblingId = nullptr;
    BTreePageId *rightSiblingId = nullptr;
    if (leftEntry != nullptr) leftSiblingId = leftEntry->getLeftChild();
    if (rightEntry != nullptr) rightSiblingId = rightEntry->getRightChild();

    int maxEmptySlots = page->getMaxEntries() - page->getMaxEntries() / 2; // ceiling
    if (leftSiblingId != nullptr) {
        auto leftSibling = dynamic_cast<BTreeInternalPage *>(getPage(tid, dirtypages, leftSiblingId, Permissions::READ_WRITE));
        // if the left sibling is at minimum occupancy, merge with it. Otherwise, steal some entries from it
        if (leftSibling->getNumEmptySlots() >= maxEmptySlots) {
            mergeInternalPages(tid, dirtypages, leftSibling, page, parent, leftEntry);
        } else {
            stealFromLeftInternalPage(tid, dirtypages, page, leftSibling, parent, leftEntry);
        }
    } else if (rightSiblingId != nullptr) {
        auto *rightSibling = dynamic_cast<BTreeInternalPage *>(getPage(tid, dirtypages, rightSiblingId, Permissions::READ_WRITE));
        // if the right sibling is at minimum occupancy, merge with it. Otherwise, steal some entries from it
        if (rightSibling->getNumEmptySlots() >= maxEmptySlots) {
            mergeInternalPages(tid, dirtypages, page, rightSibling, parent, rightEntry);
        } else {
            stealFromRightInternalPage(tid, dirtypages, page, rightSibling, parent, rightEntry);
        }
    }
}

void
BTreeFile::deleteParentEntry(TransactionId tid, PagesMap &dirtypages, BTreePage *leftPage, BTreeInternalPage *parent,
                             BTreeEntry *parentEntry) {
    // delete the entry in the parent.
    // If the parent is below minimum occupancy, get some tuples from its siblings or merge with one of the siblings
    parent->deleteKeyAndRightChild(parentEntry);
    int maxEmptySlots = parent->getMaxEntries() - parent->getMaxEntries() / 2; // ceiling
    if (parent->getNumEmptySlots() == parent->getMaxEntries()) {
        // This was the last entry in the parent.
        // In this case, the parent (root node) should be deleted, and the merged
        // page will become the new root
        BTreePageId *rootPtrId = parent->getParentId();
        // attempting to delete a non-root node
        assert(rootPtrId->getType() == BTreePageType::ROOT_PTR);
        auto *rootPtr = dynamic_cast<BTreeRootPtrPage *>(getPage(tid, dirtypages, rootPtrId, Permissions::READ_WRITE));
        leftPage->setParentId(rootPtrId);
        rootPtr->setRootId(&leftPage->getId());

        // release the parent page for reuse
        setEmptyPage(tid, dirtypages, parent->getId().pageNumber());
    } else if (parent->getNumEmptySlots() > maxEmptySlots) {
        handleMinOccupancyPage(tid, dirtypages, parent);
    }
}

Page *BTreeFile::getEmptyPage(TransactionId tid, PagesMap &dirtypages, BTreePageType type) {
    // create the new page
    int emptyPageNo = getEmptyPageNo(tid, dirtypages);
    auto *newPageId = new BTreePageId(tableid, emptyPageNo, type);

    // write empty page to disk
    auto page_size = BTreeRootPtrPage::getPageSize();
    auto *page = new uint8_t[page_size]{};
    off_t offset = BTreeRootPtrPage::getPageSize() + (emptyPageNo-1) * Database::getBufferPool().getPageSize();
    pwrite(fd, page, page_size, offset);
    delete[] page;

    // make sure the page is not in the buffer pool	or in the local cache
    Database::getBufferPool().discardPage(newPageId);
    dirtypages.erase(newPageId);

    return getPage(tid, dirtypages, newPageId, Permissions::READ_WRITE);
}

int BTreeFile::getEmptyPageNo(TransactionId tid, PagesMap &dirtypages) {
    // get a read lock on the root pointer page and use it to locate the first header page
    BTreeRootPtrPage *rootPtr = getRootPtrPage(tid, dirtypages);
    BTreePageId *headerId = rootPtr->getHeaderId();
    int emptyPageNo = 0;

    if (headerId != nullptr) {
        auto *headerPage = dynamic_cast<BTreeHeaderPage *>(getPage(tid, dirtypages, headerId,
                                                                              Permissions::READ_ONLY));
        int headerPageCount = 0;
        // try to find a header page with an empty slot
        while (headerPage != nullptr && headerPage->getEmptySlot() == -1) {
            headerId = headerPage->getNextPageId();
            if (headerId != nullptr) {
                headerPage = dynamic_cast<BTreeHeaderPage *>(getPage(tid, dirtypages, headerId,
                                                                     Permissions::READ_ONLY));
                headerPageCount++;
            } else {
                headerPage = nullptr;
            }
        }

        // if headerPage is not null, it must have an empty slot
        if (headerPage != nullptr) {
            headerPage = dynamic_cast<BTreeHeaderPage *>(getPage(tid, dirtypages, headerId,
                                                                 Permissions::READ_WRITE));
            int emptySlot = headerPage->getEmptySlot();
            headerPage->markSlotUsed(emptySlot, true);
            emptyPageNo = headerPageCount * BTreeHeaderPage::getNumSlots() + emptySlot;
        }
    }

    // at this point if headerId is null, either there are no header pages
    // or there are no free slots
    if (headerId == nullptr) {
        // create the new page
        void *emptyData = BTreeInternalPage::createEmptyPageData();
        write(fd, emptyData, Database::getBufferPool().getPageSize());
        emptyPageNo = getNumPages();
    }

    return emptyPageNo;
}

void BTreeFile::setEmptyPage(TransactionId tid, PagesMap &dirtypages, int emptyPageNo) {

    // otherwise, get a read lock on the root pointer page and use it to locate
    // the first header page
    BTreeRootPtrPage *rootPtr = getRootPtrPage(tid, dirtypages);
    const BTreePageId *headerId = rootPtr->getHeaderId();
    const BTreePageId *prevId = nullptr;
    int headerPageCount = 0;

    // if there are no header pages, create the first header page and update
    // the header pointer in the BTreeRootPtrPage
    if (headerId == nullptr) {
        rootPtr = dynamic_cast<BTreeRootPtrPage *>(getPage(tid, dirtypages, BTreeRootPtrPage::getId(tableid),
                                                           Permissions::READ_WRITE));

        auto *headerPage = dynamic_cast<BTreeHeaderPage *>(getEmptyPage(tid, dirtypages, BTreePageType::HEADER));
        headerId = &headerPage->getId();
        headerPage->init();
        rootPtr->setHeaderId(headerId);
    }

    // iterate through all the existing header pages to find the one containing the slot
    // corresponding to emptyPageNo
    while (headerId != nullptr && (headerPageCount + 1) * BTreeHeaderPage::getNumSlots() < emptyPageNo) {
        auto *headerPage = dynamic_cast<BTreeHeaderPage *>(getPage(tid, dirtypages, headerId, Permissions::READ_ONLY));
        prevId = headerId;
        headerId = headerPage->getNextPageId();
        headerPageCount++;
    }

    // at this point headerId should either be null or set with
    // the headerPage containing the slot corresponding to emptyPageNo.
    // Add header pages until we have one with a slot corresponding to emptyPageNo
    while ((headerPageCount + 1) * BTreeHeaderPage::getNumSlots() < emptyPageNo) {
        auto *prevPage = dynamic_cast<BTreeHeaderPage *>(getPage(tid, dirtypages, prevId, Permissions::READ_WRITE));

        auto *headerPage = dynamic_cast<BTreeHeaderPage *>(getEmptyPage(tid, dirtypages, BTreePageType::HEADER));
        headerId = &headerPage->getId();
        headerPage->init();
        headerPage->setPrevPageId(prevId);
        prevPage->setNextPageId(headerId);

        headerPageCount++;
        prevId = headerId;
    }

    // now headerId should be set with the headerPage containing the slot corresponding to
    // emptyPageNo
    auto *headerPage = dynamic_cast<BTreeHeaderPage *>(getPage(tid, dirtypages, headerId, Permissions::READ_WRITE));
    int emptySlot = emptyPageNo - headerPageCount * BTreeHeaderPage::getNumSlots();
    headerPage->markSlotUsed(emptySlot, false);
}

BTreeFile::BTreeFile(const char *fname, int key, const TupleDesc &td) : keyField(key), td(td) {
    fd = open(fname, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        throw std::runtime_error("open");
    }
    tableid = std::hash<std::string>{}(fname);
}

int BTreeFile::getId() const { return tableid; }

const TupleDesc &BTreeFile::getTupleDesc() const { return td; }

Page *BTreeFile::readPage(const PageId &pid) {
    const auto *id = dynamic_cast<const BTreePageId *>(&pid);
    if(id->getType() == BTreePageType::ROOT_PTR) {
        auto *pageBuf = new uint8_t[BTreeRootPtrPage::getPageSize()];
        auto retval = pread(fd, pageBuf, BTreeRootPtrPage::getPageSize(), 0);
        assert(retval != -1);
        assert(retval == BTreeRootPtrPage::getPageSize());
        return new BTreeRootPtrPage(id, pageBuf);
    }

    auto *pageBuf = new uint8_t[Database::getBufferPool().getPageSize()];
    off_t offset = BTreeRootPtrPage::getPageSize() + (id->pageNumber()-1) * Database::getBufferPool().getPageSize();
    auto retval = pread(fd, pageBuf, Database::getBufferPool().getPageSize(), offset);
    assert(retval != -1);
    assert(retval == Database::getBufferPool().getPageSize());
    if(id->getType() == BTreePageType::INTERNAL) {
        return new BTreeInternalPage(*id, pageBuf, keyField);
    }
    if(id->getType() == BTreePageType::LEAF) {
        return new BTreeLeafPage(*id, pageBuf, keyField);
    }
    // BTreePageType::HEADER
    return new BTreeHeaderPage(id, pageBuf);
}

void BTreeFile::writePage(Page *page) {
    const auto *id = dynamic_cast<const BTreePageId *>(&page->getId());

    auto data = page->getPageData();
    if(id->getType() == BTreePageType::ROOT_PTR) {
        pwrite(fd, data, BTreeRootPtrPage::getPageSize(), 0);
    } else {
        off_t offset = BTreeRootPtrPage::getPageSize() + (page->getId().pageNumber()-1) * Database::getBufferPool().getPageSize();
        pwrite(fd, data, Database::getBufferPool().getPageSize(), offset);
    }
}

int BTreeFile::getNumPages() const {
    struct stat st{};
    if (fstat(fd, &st) == -1) {
        throw std::runtime_error("fstat");
    }
    return (st.st_size - BTreeRootPtrPage::getPageSize()) / Database::getBufferPool().getPageSize();
}

int BTreeFile::getKeyField() const { return keyField; }

BTreeLeafPage *BTreeFile::findLeafPage(TransactionId tid, BTreePageId *pid, Permissions perm, const Field *f) {
    PagesMap dirtypages{};
    return findLeafPage(tid, dirtypages, pid, perm, f);
}

Page *BTreeFile::getPage(TransactionId tid, PagesMap &dirtypages, const BTreePageId *pid, Permissions perm) {
    const PageId *id = pid;
    auto it = dirtypages.find(id);
    if(it != dirtypages.end()) {
        return it->second;
    }
    else {
        Page *p = Database::getBufferPool().getPage(pid);
        if (perm == Permissions::READ_WRITE) {
            dirtypages[pid] = p;
        }
        return p;
    }
}

std::vector<Page *> BTreeFile::insertTuple(TransactionId tid, Tuple &t) {
    PagesMap dirtypages;

    // get a read lock on the root pointer page and use it to locate the root page
    BTreeRootPtrPage *rootPtr = getRootPtrPage(tid, dirtypages);
    BTreePageId *rootId = rootPtr->getRootId();

    if (rootId == nullptr) { // the root has just been created, so set the root pointer to point to it
        rootId = new BTreePageId(tableid, getNumPages(), BTreePageType::LEAF);
        rootPtr = dynamic_cast<BTreeRootPtrPage *>(getPage(tid, dirtypages, BTreeRootPtrPage::getId(tableid),
                                                           Permissions::READ_WRITE));
        rootPtr->setRootId(rootId);
    }

    // find and lock the left-most leaf page corresponding to the key field,
    // and split the leaf page if there are no more slots available
    BTreeLeafPage *leafPage = findLeafPage(tid, dirtypages, rootId, Permissions::READ_ONLY, &t.getField(keyField));
    leafPage = dynamic_cast<BTreeLeafPage *>(getPage(tid, dirtypages, &leafPage->getId(),
                                                     Permissions::READ_WRITE));
    if (leafPage->getNumEmptySlots() == 0) {
        leafPage = splitLeafPage(tid, dirtypages, leafPage, &t.getField(keyField));
    }

    // insert the tuple into the leaf page
    leafPage->insertTuple(&t);

    std::vector<Page *>dirtyPagesArr;
    for (const auto& pair : dirtypages) {
        dirtyPagesArr.push_back(pair.second);
    }
    return dirtyPagesArr;
}

std::vector<Page *> BTreeFile::deleteTuple(TransactionId tid, Tuple &t) {
    PagesMap dirtypages;

    auto *pageId = new BTreePageId(tableid, t.getRecordId()->getPageId()->pageNumber(), BTreePageType::LEAF);
    auto *page = dynamic_cast<BTreeLeafPage *>(getPage(tid, dirtypages, pageId, Permissions::READ_WRITE));
    page->deleteTuple(&t);

    // if the page is below minimum occupancy, get some tuples from its siblings
    // or merge with one of the siblings
    int maxEmptySlots = page->getMaxTuples() - page->getMaxTuples() / 2; // ceiling
    if (page->getNumEmptySlots() > maxEmptySlots) {
        handleMinOccupancyPage(tid, dirtypages, page);
    }

    std::vector<Page *> dirtyPagesArr;
    for (const auto& pair : dirtypages) {
        dirtyPagesArr.push_back(pair.second);
    }
    return dirtyPagesArr;
}

BTreeRootPtrPage *BTreeFile::getRootPtrPage(TransactionId tid, PagesMap &dirtypages) {
    struct stat st{};
    if (fstat(fd, &st) == -1) {
        throw std::runtime_error("fstat");
    }
    auto size = st.st_size;
    if(size == 0) {
        // create the root pointer page and the root page
        auto emptyRootPtrData = BTreeRootPtrPage::createEmptyPageData();
        auto emptyLeafData = BTreeLeafPage::createEmptyPageData();
        pwrite(fd, emptyRootPtrData, BTreeRootPtrPage::getPageSize(), 0);
        pwrite(fd, emptyLeafData, Database::getBufferPool().getPageSize(), BTreeRootPtrPage::getPageSize());
    }

    // get a read lock on the root pointer page
    return dynamic_cast<BTreeRootPtrPage *>(getPage(tid, dirtypages, BTreeRootPtrPage::getId(tableid),
                                                    Permissions::READ_ONLY));
}

BTreeIterable BTreeFile::iterable(TransactionId tid, IndexPredicate *ipred) {
    return {tid, ipred, this};
}


BTreeInternalPage *BTreeFile::getParentWithEmptySlots(TransactionId tid, PagesMap &dirtypages, BTreePageId *parentId, Field *field) {
    BTreeInternalPage *parent;

    // create a parent node if necessary
    // this will be the new root of the tree
    if (parentId->getType() == BTreePageType::ROOT_PTR) {
        Page *page = getEmptyPage(tid, dirtypages, BTreePageType::INTERNAL);
        parent = dynamic_cast<BTreeInternalPage *>(page);

        // update the root pointer
        auto *rootPageId = new BTreePageId(tableid, 0, BTreePageType::ROOT_PTR);
        auto *rootPtr = (BTreeRootPtrPage *) getPage(tid, dirtypages, rootPageId, Permissions::READ_WRITE);
        BTreePageId *prevRootId = rootPtr->getRootId(); //save prev id before overwriting.
        rootPtr->setRootId(dynamic_cast<const BTreePageId *>(&parent->getId()));

        // update the previous root to now point to this new root.
        page = getPage(tid, dirtypages, prevRootId, Permissions::READ_WRITE);
        auto *prevRootPage = dynamic_cast<BTreePage *>(page);
        prevRootPage->setParentId(dynamic_cast<const BTreePageId *>(&parent->getId()));
    } else {
        // lock the parent page
        Page *page = getPage(tid, dirtypages, parentId, Permissions::READ_WRITE);
        parent = dynamic_cast<BTreeInternalPage *>(page);
    }

    // split the parent if needed
    if (parent->getNumEmptySlots() == 0) {
        parent = splitInternalPage(tid, dirtypages, parent, field);
    }

    return parent;
}

void BTreeFile::updateParentPointers(TransactionId tid, PagesMap &dirtypages,
                                     BTreeInternalPage *page) {
    auto it = page->begin();
    auto end = page->end();
    const BTreePageId &pid = page->getId();
    BTreeEntry *e = nullptr;
    while (it != end) {
        e = &*it;
        updateParentPointer(tid, dirtypages, &pid, e->getLeftChild());
    }
    if (e != nullptr) {
        updateParentPointer(tid, dirtypages, &pid, e->getRightChild());
    }
}
