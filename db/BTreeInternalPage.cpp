#include <db/BTreeInternalPage.h>
#include <db/Database.h>

#include <cstring>
#include <cassert>

using namespace db;

BTreeInternalPageIterator::BTreeInternalPageIterator(int slot, BTreeInternalPage *page) {
    p = page;
    curEntry = slot;
    if (slot == p->numSlots) {
        return;
    }
    currChildId = p->getChildId(curEntry);

    if(currChildId == nullptr) {
        curEntry = p->numSlots;
    }
}

bool BTreeInternalPageIterator::operator!=(const BTreeInternalPageIterator &other) const {
    return curEntry != other.curEntry;
}

BTreeInternalPageIterator &BTreeInternalPageIterator::operator++() {
    prevChildId = currChildId;
    while (++curEntry < p->numSlots) {
        if (p->isSlotUsed(curEntry)) {
            currChildId = p->getChildId(curEntry);
            break;
        }
    }
    return *this;
}

BTreeEntry &BTreeInternalPageIterator::operator*() const {
    Field *key = p->getKey(curEntry);
    BTreeEntry *entry = new BTreeEntry(key, prevChildId, currChildId);
    entry->setRecordId(new RecordId(&p->pid, curEntry));
    return *entry;
}

BTreeInternalPageReverseIterator::BTreeInternalPageReverseIterator(int slot, BTreeInternalPage *page) {
    p = page;
    leftSlot = slot;  // p->numSlots - 1;
    rightSlot = slot;
    while (leftSlot > 0 && !p->isSlotUsed(leftSlot)) {
        --leftSlot;
    }
    if (leftSlot > 0) {
        leftChildId = p->getChildId(leftSlot);
    } else {
        leftChildId = nullptr;
    }
    rightChildId = nullptr;
}

bool BTreeInternalPageReverseIterator::operator!=(const BTreeInternalPageReverseIterator &other) const {
    return leftSlot != other.leftSlot;
}

BTreeInternalPageReverseIterator &BTreeInternalPageReverseIterator::operator++() {
    rightChildId = leftChildId;
    rightSlot = leftSlot;
    while (--leftSlot >= 0) {
        if (p->isSlotUsed(leftSlot)) {
            leftChildId = p->getChildId(leftSlot);
            break;
        }
    }
    return *this;
}

BTreeEntry &BTreeInternalPageReverseIterator::operator*() const {
    Field *key = p->getKey(rightSlot);
    BTreeEntry *entry = new BTreeEntry(key, leftChildId, rightChildId);
    entry->setRecordId(new RecordId(&p->pid, rightSlot));
    return *entry;
}

int BTreeInternalPage::getHeaderSize() const {
    int slotsPerPage = getMaxEntries() + 1;
    int hb = (slotsPerPage / 8);
    if (hb * 8 < slotsPerPage) hb++;

    return hb;
}

void BTreeInternalPage::deleteEntry(BTreeEntry *e, bool deleteRightChild) {
    RecordId *rid = e->getRecordId();
    // tried to delete entry with nullptr rid
    assert(rid != nullptr);
    // tried to delete entry on invalid page or table
    assert(rid->getPageId()->pageNumber() == pid.pageNumber() &&
           rid->getPageId()->getTableId() != pid.getTableId());
    // tried to delete nullptr entry.
    assert(isSlotUsed(rid->getTupleno()));
    if (deleteRightChild) {
        markSlotUsed(rid->getTupleno(), false);
    } else {
        for (int i = rid->getTupleno() - 1; i >= 0; i--) {
            if (isSlotUsed(i)) {
                children[i] = children[rid->getTupleno()];
                markSlotUsed(rid->getTupleno(), false);
                break;
            }
        }
    }
    e->setRecordId(nullptr);
}

void BTreeInternalPage::moveEntry(int from, int to) {
    if (!isSlotUsed(to) && isSlotUsed(from)) {
        markSlotUsed(to, true);
        keys[to] = keys[from];
        children[to] = children[from];
        markSlotUsed(from, false);
    }
}

void BTreeInternalPage::markSlotUsed(int i, bool value) {
    int headerbit = i % 8;
    int headerbyte = (i - headerbit) / 8;

    if (value)
        header[headerbyte] |= 1 << headerbit;
    else
        header[headerbyte] &= (0xFF ^ (1 << headerbit));
}

Field *BTreeInternalPage::getKey(int i) {

    // key at slot 0 is not used
    assert(0 < i && i < numSlots);

    if (!isSlotUsed(i)) {
        return nullptr;
    }

    return keys[i];
}

BTreePageId *BTreeInternalPage::getChildId(int i) {
    assert(0 <= i && i < numSlots);

    if (!isSlotUsed(i)) {
        return nullptr;
    }

    return new BTreePageId(pid.getTableId(), children[i], childCategory);
}

BTreeInternalPage::BTreeInternalPage(const BTreePageId &id, uint8_t *data, int key) : BTreePage(id, key) {
    numSlots = getMaxEntries() + 1;

    // Read the parent pointer
    size_t offset = 0;
    memcpy(&parent, data + offset, sizeof(int));
    offset += sizeof(int);

    // read the child page category
    memcpy(&childCategory, data + offset, sizeof(BTreePageType));
    offset += sizeof(BTreePageType);

    // allocate and read the header slots of this page
    auto header_size = getHeaderSize();
    header = new uint8_t[header_size];
    memcpy(header, data + offset, header_size);
    offset += header_size;

    keys = new Field *[numSlots];
    // allocate and read the keys of this page
    // start from 1 because the first key slot is not used
    // since a node with m keys has m+1 pointers
    keys[0] = nullptr;
    for (int i = 1; i < numSlots; i++) {
        auto type = td.getFieldType(keyField);
        keys[i] = Types::parse(data + offset, type);
        offset += Types::getLen(type);
    }

    children = new int[numSlots];
    // allocate and read the child pointers of this page
    for (int i = 0; i < numSlots; i++) {
        children[i] = *(int *) data;
    }
    childCategory = BTreePageType::LEAF;
}

int BTreeInternalPage::getMaxEntries() const {
    size_t keySize = Types::getLen(td.getFieldType(keyField));
    // extraBits are: parent pointer, child page category, extra child pointer (node with m entries has m+1 pointers to children), 1 bit for extra header
    return ((Database::getBufferPool().getPageSize() - 2 * sizeof(int) - sizeof(BTreePageType)) * 8 + 1) /
           ((keySize + sizeof(int)) * 8 + 1); //round down
}

void *BTreeInternalPage::getPageData() const {
    auto *data = (uint8_t *) createEmptyPageData();
    size_t offset = 0;
    memcpy(data + offset, &parent, sizeof(int));
    offset += sizeof(int);
    memcpy(data + offset, &childCategory, sizeof(BTreePageType));
    offset += sizeof(BTreePageType);
    auto header_size = getHeaderSize();
    memcpy(data + offset, header, header_size);
    offset += header_size;
    size_t key_size = Types::getLen(td.getFieldType(keyField));
    for (int i = 1; i < numSlots; i++) {
        if (isSlotUsed(i)) {
            keys[i]->serialize(data + offset);
        }
        offset += key_size;
    }

    // create the child pointers
    for (int i = 0; i < numSlots; i++) {
        if (isSlotUsed(i)) {
            memcpy(data + offset, &children[i], sizeof(int));
        }
        offset += sizeof(int);
    }

    return data;
}

void BTreeInternalPage::deleteKeyAndRightChild(BTreeEntry *e) {
    deleteEntry(e, true);
}

void BTreeInternalPage::deleteKeyAndLeftChild(BTreeEntry *e) {
    deleteEntry(e, false);
}

void BTreeInternalPage::updateEntry(BTreeEntry *e) {
    RecordId *rid = e->getRecordId();
    // tried to update entry with null rid
    assert(rid != nullptr);
    // tried to update entry on invalid page or table
    assert(rid->getPageId()->pageNumber() == pid.pageNumber() &&
           rid->getPageId()->getTableId() == pid.getTableId());
    // tried to update null entry.
    assert(isSlotUsed(rid->getTupleno()));

    for (int i = rid->getTupleno() + 1; i < numSlots; i++) {
        if (isSlotUsed(i)) {
            // attempt to update entry with invalid key e.getKey(). HINT: updated key must be less than or equal to keys on the right
            assert(keys[i]->compare(Op::GREATER_THAN_OR_EQ, e->getKey()));
            break;
        }
    }
    for (int i = rid->getTupleno() - 1; i >= 0; i--) {
        if (isSlotUsed(i)) {
            // attempt to update entry with invalid key e.getKey(). HINT: updated key must be greater than or equal to keys on the left
            assert(i == 0 || keys[i]->compare(Op::LESS_THAN_OR_EQ, e->getKey()));
            children[i] = e->getLeftChild()->pageNumber();
            break;
        }
    }
    children[rid->getTupleno()] = e->getRightChild()->pageNumber();
    keys[rid->getTupleno()] = e->getKey();
}

void BTreeInternalPage::insertEntry(BTreeEntry e) {
    // key field type mismatch, in insertEntry
    assert(e.getKey()->getType() == td.getFieldType(keyField));
    // table id mismatch in insertEntry
    assert(e.getLeftChild()->getTableId() == pid.getTableId() &&
           e.getRightChild()->getTableId() == pid.getTableId());

    if (childCategory == BTreePageType::ROOT_PTR) {
        // child page category mismatch in insertEntry
        assert(e.getLeftChild()->getType() == e.getRightChild()->getType());
        childCategory = e.getLeftChild()->getType();
    } else {
        // child page category mismatch in insertEntry
        assert(e.getLeftChild()->getType() == childCategory && e.getRightChild()->getType() == childCategory);
    }

    // if this is the first entry, add it and return
    if (getNumEmptySlots() == getMaxEntries()) {
        children[0] = e.getLeftChild()->pageNumber();
        children[1] = e.getRightChild()->pageNumber();
        keys[1] = e.getKey();
        markSlotUsed(0, true);
        markSlotUsed(1, true);
        e.setRecordId(new RecordId(&pid, 1));
        return;
    }

    // find the first empty slot, starting from 1
    int emptySlot = -1;
    for (int i = 1; i < numSlots; i++) {
        if (!isSlotUsed(i)) {
            emptySlot = i;
            break;
        }
    }

    // called insertEntry on page with no empty slots.
    assert(emptySlot != -1);

    // find the child pointer matching the left or right child in this entry
    int lessOrEqKey = -1;
    for (int i = 0; i < numSlots; i++) {
        if (isSlotUsed(i)) {
            if (children[i] == e.getLeftChild()->pageNumber() ||
                children[i] == e.getRightChild()->pageNumber()) {
                // attempt to insert invalid entry with
                // HINT: one of these children must match an existing child on the page and
                // this key must be correctly ordered in between that child's left and right keys
                assert(i == 0 || keys[i]->compare(Op::LESS_THAN_OR_EQ, e.getKey()));
                lessOrEqKey = i;
                if (children[i] == e.getRightChild()->pageNumber()) {
                    children[i] = e.getLeftChild()->pageNumber();
                }
            } else if (lessOrEqKey != -1) {
                // validate that the next key is greater than or equal to the one we are inserting
                assert(keys[i]->compare(Op::GREATER_THAN_OR_EQ, e.getKey()));
                // attempt to insert invalid entry with left child
                // HINT: one of these children must match an existing child on the page
                // and this key must be correctly ordered in between that child's
                // left and right keys
                break;
            }
        }
    }

    assert(lessOrEqKey != -1);
    // attempt to insert invalid entry
    // HINT: one of these children must match an existing child on the page
    // and this key must be correctly ordered in between that child's
    // left and right keys

    // shift entries back or forward to fill empty slot and make room for new entry
    // while keeping entries in sorted order
    int goodSlot;
    if (emptySlot < lessOrEqKey) {
        for (int i = emptySlot; i < lessOrEqKey; i++) {
            moveEntry(i + 1, i);
        }
        goodSlot = lessOrEqKey;
    } else {
        for (int i = emptySlot; i > lessOrEqKey + 1; i--) {
            moveEntry(i - 1, i);
        }
        goodSlot = lessOrEqKey + 1;
    }

    // insert new entry into the correct spot in sorted order
    markSlotUsed(goodSlot, true);
    keys[goodSlot] = e.getKey();
    children[goodSlot] = e.getRightChild()->pageNumber();
    e.setRecordId(new RecordId(&pid, goodSlot));
}

int BTreeInternalPage::getNumEntries() const {
    return numSlots - getNumEmptySlots() - 1;
}

int BTreeInternalPage::getNumEmptySlots() const {
    int cnt = 0;
    // start from 1 because the first key slot is not used
    // since a node with m keys has m+1 pointers
    for (int i = 1; i < numSlots; i++)
        if (!isSlotUsed(i))
            cnt++;
    return cnt;
}

bool BTreeInternalPage::isSlotUsed(int i) const {
    int headerbit = i % 8;
    int headerbyte = (i - headerbit) / 8;
    return (header[headerbyte] & (1 << headerbit)) != 0;
}

BTreeInternalPageIterator BTreeInternalPage::begin() {
    return {1, this};
}

BTreeInternalPageIterator BTreeInternalPage::end() {
    return {numSlots, this};
}

BTreeInternalPageReverseIterator BTreeInternalPage::rbegin() {
    return {numSlots - 1, this};
}

BTreeInternalPageReverseIterator BTreeInternalPage::rend() {
    return {0, this};
}
