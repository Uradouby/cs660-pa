#include <db/TupleDesc.h>
#include <stdexcept>

using namespace db;

//
// TDItem
//

bool TDItem::operator==(const TDItem &other) const {
    return fieldType == other.fieldType && fieldName == other.fieldName;
}

std::size_t std::hash<TDItem>::operator()(const TDItem &r) const {
    return std::hash<Types::Type>()(r.fieldType) ^ std::hash<std::string>()(r.fieldName);
}

//
// TupleDesc
//

TupleDesc::TupleDesc(const std::vector<Types::Type> &types) {
    size = 0;
    for (const auto &type: types) {
        items.emplace_back(type, "");
        size += Types::getLen(type);
    }
}

TupleDesc::TupleDesc(const std::vector<Types::Type> &types, const std::vector<std::string> &names) {
    size = 0;
    for (int i = 0; i < types.size(); ++i) {
        items.emplace_back(types[i], names[i]);
        size += Types::getLen(types[i]);
    }
}

size_t TupleDesc::numFields() const {
    return items.size();
}

std::string TupleDesc::getFieldName(size_t i) const {
    return items[i].fieldName;
}

Types::Type TupleDesc::getFieldType(size_t i) const {
    return items[i].fieldType;
}

int TupleDesc::fieldNameToIndex(const std::string &fieldName) const {
    for (int i = 0; i < items.size(); ++i) {
        if (items[i].fieldName == fieldName) {
            return i;
        }
    }
    throw std::invalid_argument("field name not found");
}

size_t TupleDesc::getSize() const {
    return size;
}

TupleDesc TupleDesc::merge(const TupleDesc &td1, const TupleDesc &td2) {
    std::vector<Types::Type> types;
    std::vector<std::string> names;
    for (const auto &item: td1.items) {
        types.push_back(item.fieldType);
        names.push_back(item.fieldName);
    }
    for (const auto &item: td2.items) {
        types.push_back(item.fieldType);
        names.push_back(item.fieldName);
    }
    return TupleDesc{types, names};
}

std::string TupleDesc::to_string() const {
    if (items.empty()) {
        return "";
    }
    std::string ret = items[0].to_string();
    for (size_t i = 1; i < items.size(); i++) {
        ret += items[i].to_string() + ", ";
    }
    return ret;
}

bool TupleDesc::operator==(const TupleDesc &other) const {
    return items == other.items;
}

TupleDesc::iterator TupleDesc::begin() const {
    return items.begin();
}

TupleDesc::iterator TupleDesc::end() const {
    return items.end();
}

std::size_t std::hash<db::TupleDesc>::operator()(const db::TupleDesc &td) const {
    size_t seed = 0;
    for (size_t i = 0; i < td.numFields(); i++) {
        const auto &type = td.getFieldType(i);
        seed += std::hash<Types::Type>()(type);
    }
    return seed;
}
