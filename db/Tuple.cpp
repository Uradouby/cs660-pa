#include <db/Tuple.h>

using namespace db;

//
// Tuple
//

// TODO pa1.1: implement
Tuple::Tuple(const TupleDesc &td, RecordId *rid) {
}

const TupleDesc &Tuple::getTupleDesc() const {
    // TODO pa1.1: implement
}

const RecordId *Tuple::getRecordId() const {
    // TODO pa1.1: implement
}

void Tuple::setRecordId(const RecordId *id) {
    // TODO pa1.1: implement
}

const Field &Tuple::getField(int i) const {
    // TODO pa1.1: implement
}

void Tuple::setField(int i, const Field *f) {
    // TODO pa1.1: implement
}

Tuple::iterator Tuple::begin() const {
    // TODO pa1.1: implement
}

Tuple::iterator Tuple::end() const {
    // TODO pa1.1: implement
}

std::string Tuple::to_string() const {
    // TODO pa1.1: implement
}
