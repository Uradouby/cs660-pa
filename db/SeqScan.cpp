#include <db/SeqScan.h>

using namespace db;

// TODO pa1.6: implement
SeqScan::SeqScan(TransactionId *tid, int tableid, const std::string &tableAlias) {
}

std::string SeqScan::getTableName() const {
    // TODO pa1.6: implement
}

std::string SeqScan::getAlias() const {
    // TODO pa1.6: implement
}

void SeqScan::reset(int tabid, const std::string &tableAlias) {
    // TODO pa1.6: implement
}

const TupleDesc &SeqScan::getTupleDesc() const {
    // TODO pa1.6: implement
}

SeqScan::iterator SeqScan::begin() const {
    // TODO pa1.6: implement
}

SeqScan::iterator SeqScan::end() const {
    // TODO pa1.6: implement
}

//
// SeqScanIterator
//

// TODO pa1.6: implement
SeqScanIterator::SeqScanIterator(/* TODO pa1.6: add parameters */) {
}

bool SeqScanIterator::operator!=(const SeqScanIterator &other) const {
    // TODO pa1.6: implement
}

SeqScanIterator &SeqScanIterator::operator++() {
    // TODO pa1.6: implement
}

const Tuple &SeqScanIterator::operator*() const {
    // TODO pa1.6: implement
}
