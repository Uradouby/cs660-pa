#include <db/BTreeEntry.h>

using namespace db;

BTreeEntry::BTreeEntry(Field *field, BTreePageId *lcid, BTreePageId *rcid) {
    key = field;
    leftChild = lcid;
    rightChild = rcid;
    rid = nullptr;
}

Field *BTreeEntry::getKey() const {
    return key;
}

BTreePageId *BTreeEntry::getLeftChild() const {
    return leftChild;
}

BTreePageId *BTreeEntry::getRightChild() const {
    return rightChild;
}

RecordId *BTreeEntry::getRecordId() const {
    return rid;
}

void BTreeEntry::setKey(Field *field) {
    key = field;
}

void BTreeEntry::setLeftChild(BTreePageId *id) {
    leftChild = id;
}

void BTreeEntry::setRightChild(BTreePageId *id) {
    rightChild = id;
}

void BTreeEntry::setRecordId(RecordId *id) {
    rid = id;
}

std::string BTreeEntry::to_string() const {
    return "[" + std::to_string(leftChild->pageNumber()) + "|" + key->to_string() + "|" +
           std::to_string(rightChild->pageNumber()) + "]";
}
