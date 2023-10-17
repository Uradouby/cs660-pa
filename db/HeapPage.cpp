#include <db/HeapPage.h>
#include <db/Database.h>

using namespace db;

HeapPageIterator::HeapPageIterator(int i, const HeapPage *page) {
    this->slot = i;
    this->page = page;
    while (slot < page->numSlots && !page->isSlotUsed(slot)) {
        slot++;
    }
}

bool HeapPageIterator::operator!=(const HeapPageIterator &other) const {
    return slot != other.slot || page != other.page;
}

HeapPageIterator &HeapPageIterator::operator++() {
    do {
        slot++;
    } while (slot < page->numSlots && !page->isSlotUsed(slot));
    return *this;
}

Tuple &HeapPageIterator::operator*() const {
    return page->tuples[slot];
}

HeapPage::HeapPage(const HeapPageId &id, uint8_t *data) : pid(id) {
    this->td = Database::getCatalog().getTupleDesc(id.getTableId());
    this->numSlots = getNumTuples();

    // Allocate and read the header slots of this page
    size_t header_size = getHeaderSize();
    header = new uint8_t[header_size];
    memcpy(header, data, header_size);
    size_t offset = header_size;

    tuples = new Tuple[numSlots];
    size_t tuple_size = td.getSize();
    for (int slot = 0; slot < numSlots; slot++) {
        if (isSlotUsed(slot)) {
            readTuple(tuples + slot, data + offset, slot);
        }
        offset += tuple_size;
    }
}

int HeapPage::getNumTuples() const {
    return Database::getBufferPool().getPageSize() * 8 / (td.getSize() * 8 + 1);
}

int HeapPage::getHeaderSize() const {
    return Database::getBufferPool().getPageSize() - td.getSize() * numSlots;
}

const PageId &HeapPage::getId() const {
    return pid;
}

void HeapPage::readTuple(Tuple *t, uint8_t *data, int slotId) {
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

void *HeapPage::getPageData() const {
    auto *data = (uint8_t *) createEmptyPageData();
    size_t header_size = getHeaderSize();

    // Create the header of the page
    memcpy(data, header, header_size);
    size_t offset = header_size;
    // Create the tuples
    for (int i = 0; i < numSlots; i++) {
        const Tuple &tuple = tuples[i];
        // Empty slot
        if (!isSlotUsed(i)) {
            memset(data + offset, 0, td.getSize());
            offset += td.getSize();
        } else {
            // Non-empty slot
            for (int j = 0; j < td.numFields(); j++) {
                const Field &f = tuple.getField(j);
                f.serialize(data + offset);
                offset += Types::getLen(f.getType());
            }
        }
    }

    return data;
}

void *HeapPage::createEmptyPageData() {
    int len = Database::getBufferPool().getPageSize();
    return new uint8_t[len]{}; // all 0
}

int HeapPage::getNumEmptySlots() const {
    int cnt = 0;
    for (int i = 0; i < numSlots; i++)
        if (!isSlotUsed(i)) cnt++;
    return cnt;
}

bool HeapPage::isSlotUsed(int i) const {
    int headerbyte = i >> 3;
    int headerbit = i & 0b111;
    return (header[headerbyte] >> headerbit & 1) != 0;
}

HeapPageIterator HeapPage::begin() const {
    return HeapPageIterator(0, this);
}

HeapPageIterator HeapPage::end() const {
    return HeapPageIterator(numSlots, this);
}
