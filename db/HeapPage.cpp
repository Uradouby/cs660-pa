#include <db/HeapPage.h>

using namespace db;

void HeapPage::markSlotUsed(int i, bool value) {
    int headerbyte = i >> 3;
    int headerbit = i & 0b111;
    if (value)
        header[headerbyte] |= 1 << headerbit;
    else
        header[headerbyte] &= (0xFF ^ (1 << headerbit));
}

void HeapPage::deleteTuple(Tuple *t) {
    int tupleNumber = t->getRecordId()->getTupleno();
    if (*t->getRecordId()->getPageId() != pid) {
        throw std::runtime_error("Wrong page");
    }
    if (!isSlotUsed(tupleNumber)) {
        throw std::runtime_error("Empty slot");
    }
    markSlotUsed(tupleNumber, false);
}

void HeapPage::insertTuple(Tuple *t) {
    if (getNumEmptySlots() <= 0) {
        throw std::runtime_error("No space");
    }
    if (td != t->getTupleDesc()) {
        throw std::runtime_error("Wrong tuple description");
    }

    int slotIndex = 0;
    while (isSlotUsed(slotIndex)) {
        slotIndex++;
    }
    markSlotUsed(slotIndex, true);
    t->setRecordId(new RecordId(&pid, slotIndex));
    tuples[slotIndex] = *t;
}
