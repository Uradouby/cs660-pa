#include <db/BTreeLeafPage.h>
#include <db/Database.h>
#include <db/Catalog.h>
#include <db/BufferPool.h>
#include <cassert>

using namespace db;

BTreeLeafPageIterator::BTreeLeafPageIterator(int index, BTreeLeafPage *page, bool reverse) {
    this->page = page;
    this->reverse = reverse;
    this->index = index;
}

bool BTreeLeafPageIterator::operator!=(const BTreeLeafPageIterator &other) const {
    return index != other.index;
}

BTreeLeafPageIterator &BTreeLeafPageIterator::operator++() {
    if (reverse) {
        while (--index >= 0) {
            if (page->isSlotUsed(index)) {
                break;
            }
        }
    } else {
        while (++index < page->numSlots) {
            if (page->isSlotUsed(index)) {
                break;
            }
        }
    }
    return *this;
}

Tuple &BTreeLeafPageIterator::operator*() const {
    return *page->getTuple(index);
}

size_t BTreeLeafPage::getHeaderSize() const {
    return Database::getBufferPool().getPageSize() - td.getSize() * numSlots;
}

void BTreeLeafPage::readTuple(Tuple *t, uint8_t *data, int slotId) {
    new(t) Tuple(td, new RecordId(&pid, slotId));
    int i = 0;
    for (const auto &item: td) {
        Types::Type type = item.fieldType;
        const Field *f = Types::parse(data, type);
        data += Types::getLen(type);
        t->setField(i, f);
        i++;
    }
}

void BTreeLeafPage::moveRecord(int from, int to) {
    if (!isSlotUsed(to) && isSlotUsed(from)) {
        markSlotUsed(to, true);
        auto *rid = new RecordId(&pid, to);
        tuples[to] = tuples[from];
        tuples[to].setRecordId(rid);
        markSlotUsed(from, false);
    }
}

void BTreeLeafPage::markSlotUsed(int i, bool value) {
    int headerbit = i % 8;
    int headerbyte = (i - headerbit) / 8;

    if (value)
        header[headerbyte] |= 1 << headerbit;
    else
        header[headerbyte] &= (0xFF ^ (1 << headerbit));
}

BTreeLeafPage::BTreeLeafPage(const BTreePageId &id, uint8_t *data, int key) : BTreePage(id, key) {
    numSlots = getMaxTuples();
    int *int_data = (int *) data;
    parent = int_data[0];
    leftSibling = int_data[1];
    rightSibling = int_data[2];
    // allocate and read the header slots of this page
    auto header_size = getHeaderSize();
    header = new uint8_t[header_size];
    memcpy(header, int_data + 3, header_size);
    memcpy(header, int_data + 3, header_size);
    size_t offset = 3 * sizeof(int) + header_size;

    tuples = new Tuple[numSlots];
    readTuples(data + offset);
}

void BTreeLeafPage::readTuples(uint8_t *data) {
    size_t offset = 0;
    size_t tuple_size = td.getSize();
    for (int slot = 0; slot < numSlots; slot++) {
        if (isSlotUsed(slot)) {
            readTuple(tuples + slot, data + offset, slot);
        }
        offset += tuple_size;
    }
}

size_t BTreeLeafPage::getMaxTuples() const {
    return (Database::getBufferPool().getPageSize() - 3 * sizeof(int)) * 8 / (td.getSize() * 8 + 1); // round down
}

void BTreeLeafPage::deleteTuple(Tuple *t) {
    const RecordId *rid = t->getRecordId();
    // tried to delete tuple with null rid
    assert(rid != nullptr);
    // tried to delete tuple on invalid page or table
    assert(rid->getPageId()->pageNumber() == pid.pageNumber() &&
           rid->getPageId()->getTableId() == pid.getTableId());
    // tried to delete null tuple.");
    assert(isSlotUsed(rid->getTupleno()));
    markSlotUsed(rid->getTupleno(), false);
    t->setRecordId(nullptr);
}

void BTreeLeafPage::insertTuple(Tuple *t) {
    // type mismatch, in addTuple
    assert(t->getTupleDesc() == td);

    // find the first empty slot
    int emptySlot = -1;
    for (int i = 0; i < numSlots; i++) {
        if (!isSlotUsed(i)) {
            emptySlot = i;
            break;
        }
    }

    // called addTuple on page with no empty slots.
    assert(emptySlot != -1);

    // find the last key less than or equal to the key being inserted
    int lessOrEqKey = -1;
    const Field &key = t->getField(keyField);
    for (int i = 0; i < numSlots; i++) {
        if (isSlotUsed(i)) {
            if (!tuples[i].getField(keyField).compare(Op::LESS_THAN_OR_EQ, &key))
                break;
            lessOrEqKey = i;
        }
    }

    // shift records back or forward to fill empty slot and make room for new record
    // while keeping records in sorted order
    int goodSlot;
    if (emptySlot < lessOrEqKey) {
        for (int i = emptySlot; i < lessOrEqKey; i++) {
            moveRecord(i + 1, i);
        }
        goodSlot = lessOrEqKey;
    } else {
        for (int i = emptySlot; i > lessOrEqKey + 1; i--) {
            moveRecord(i - 1, i);
        }
        goodSlot = lessOrEqKey + 1;
    }

    // insert new record into the correct spot in sorted order
    markSlotUsed(goodSlot, true);
    auto *rid = new RecordId(&pid, goodSlot);
    t->setRecordId(rid);
    tuples[goodSlot] = *t;
}

BTreePageId *BTreeLeafPage::getLeftSiblingId() {
    if (leftSibling == 0) {
        return nullptr;
    }
    return new BTreePageId(pid.getTableId(), leftSibling, BTreePageType::LEAF);
}

BTreePageId *BTreeLeafPage::getRightSiblingId() {
    if (rightSibling == 0) {
        return nullptr;
    }
    return new BTreePageId(pid.getTableId(), rightSibling, BTreePageType::LEAF);
}

void BTreeLeafPage::setLeftSiblingId(BTreePageId *id) {
    if (id == nullptr) {
        leftSibling = 0;
    } else {
        // table id mismatch in setLeftSiblingId
        assert(id->getTableId() == pid.getTableId());
        // leftSibling must be a leaf node
        assert(id->getType() == BTreePageType::LEAF);
        leftSibling = id->pageNumber();
    }
}

void BTreeLeafPage::setRightSiblingId(BTreePageId *id) {
    if (id == nullptr) {
        rightSibling = 0;
    } else {
        // table id mismatch in setRightSiblingId
        assert(id->getTableId() == pid.getTableId());
        // rightSibling must be a leaf node
        assert(id->getType() == BTreePageType::LEAF);
        rightSibling = id->pageNumber();
    }
}

int BTreeLeafPage::getNumTuples() {
    return numSlots - getNumEmptySlots();
}

int BTreeLeafPage::getNumEmptySlots() const {
    int cnt = 0;
    for (int i = 0; i < numSlots; i++)
        if (!isSlotUsed(i))
            cnt++;
    return cnt;
}

bool BTreeLeafPage::isSlotUsed(int i) const {
    int headerbit = i % 8;
    int headerbyte = (i - headerbit) / 8;
    return (header[headerbyte] & (1 << headerbit)) != 0;
}

BTreeLeafPageIterator BTreeLeafPage::begin() {
    int index = 0;
    while (index < numSlots) {
        if (isSlotUsed(index)) {
            break;
        }
        index++;
    }
    return {index, this};
}

BTreeLeafPageIterator BTreeLeafPage::end() {
    return {numSlots, this};
}

BTreeLeafPageIterator BTreeLeafPage::rbegin() {
    int index = numSlots - 1;
    while (index >= 0) {
        if (isSlotUsed(index)) {
            break;
        }
        index--;
    }
    return {index, this, true};
}

BTreeLeafPageIterator BTreeLeafPage::rend() {
    return {-1, this, true};
}

Tuple *BTreeLeafPage::getTuple(int i) {
    assert(i < numSlots);
    if (!isSlotUsed(i)) {
        return nullptr;
    }
    return tuples + i;
}

void *BTreeLeafPage::getPageData() const {
    uint8_t *data = (uint8_t *) createEmptyPageData();
    // write out the parent and sibling pointers
    size_t offset = 0;
    memcpy(data + offset, &parent, sizeof(int));
    offset += sizeof(int);

    memcpy(data + offset, &leftSibling, sizeof(int));
    offset += sizeof(int);

    memcpy(data + offset, &rightSibling, sizeof(int));
    offset += sizeof(int);

    auto header_size = getHeaderSize();
    memcpy(data + offset, header, header_size);
    offset += header_size;

    auto td_size = td.getSize();
    // create the tuples
    for (int i = 0; i < numSlots; i++) {
        // empty slot
        if (isSlotUsed(i)) {
            // non-empty slot
            for (int j = 0; j < td.numFields(); j++) {
                const Field &f = tuples[i].getField(j);
                f.serialize(data + offset);
            }
        }
        offset += td_size;
    }
    return data;
}
