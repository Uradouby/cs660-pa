#include <db/TupleDesc.h>

using namespace db;

//
// TDItem
//

bool TDItem::operator==(const TDItem &other) const {
    // TODO pa1.1: implement
}

std::size_t std::hash<TDItem>::operator()(const TDItem &r) const {
    // TODO pa1.1: implement
}

//
// TupleDesc
//

// TODO pa1.1: implement
TupleDesc::TupleDesc(const std::vector<Types::Type> &types) {
}

// TODO pa1.1: implement
TupleDesc::TupleDesc(const std::vector<Types::Type> &types, const std::vector<std::string> &names) {
}

size_t TupleDesc::numFields() const {
    // TODO pa1.1: implement
}

std::string TupleDesc::getFieldName(size_t i) const {
    // TODO pa1.1: implement
}

Types::Type TupleDesc::getFieldType(size_t i) const {
    // TODO pa1.1: implement
}

int TupleDesc::fieldNameToIndex(const std::string &fieldName) const {
    // TODO pa1.1: implement
}

size_t TupleDesc::getSize() const {
    // TODO pa1.1: implement
}

TupleDesc TupleDesc::merge(const TupleDesc &td1, const TupleDesc &td2) {
    // TODO pa1.1: implement
}

std::string TupleDesc::to_string() const {
    // TODO pa1.1: implement
}

bool TupleDesc::operator==(const TupleDesc &other) const {
    // TODO pa1.1: implement
}

TupleDesc::iterator TupleDesc::begin() const {
    // TODO pa1.1: implement
}

TupleDesc::iterator TupleDesc::end() const {
    // TODO pa1.1: implement
}

std::size_t std::hash<db::TupleDesc>::operator()(const db::TupleDesc &td) const {
    // TODO pa1.1: implement
}
