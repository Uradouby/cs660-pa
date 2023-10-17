#include <db/IndexPredicate.h>

using namespace db;

IndexPredicate::IndexPredicate(Op op, const Field *fvalue) {
    // TODO pa2.2: implement
}

const Field *IndexPredicate::getField() const {
    // TODO pa2.2: implement
    return {};
}

Op IndexPredicate::getOp() const {
    // TODO pa2.2: implement
    return {};
}

bool IndexPredicate::operator==(const IndexPredicate &other) const {
    // TODO pa2.2: implement
    return {};
}

std::size_t std::hash<IndexPredicate>::operator()(const IndexPredicate &ipd) const {
    // TODO pa2.2: implement
    return {};
}
