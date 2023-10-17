#include <db/Tuple.h>

using namespace db;

//
// Tuple
//

Tuple::Tuple(const TupleDesc &td, RecordId *rid) : td(td), rid(rid) {
    fields.resize(td.numFields());
}

const TupleDesc &Tuple::getTupleDesc() const {
    return td;
}

const RecordId *Tuple::getRecordId() const {
    return rid;
}

void Tuple::setRecordId(const RecordId *id) {
    rid = id;
}

const Field &Tuple::getField(int i) const {
    return *fields[i];
}

void Tuple::setField(int i, const Field *f) {
    fields[i] = f;
}

Tuple::iterator Tuple::begin() const {
    return fields.begin();
}

Tuple::iterator Tuple::end() const {
    return fields.end();
}

std::string Tuple::to_string() const {
    std::string s;
    auto it = fields.begin();
    if (it != fields.end()) {
        s += (*it)->to_string();
        ++it;
    }
    while (it != fields.end()) {
        s += ", " + (*it)->to_string();
        ++it;
    }
    return s;
}
