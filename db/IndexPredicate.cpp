#include <db/IndexPredicate.h>

using namespace db;

IndexPredicate::IndexPredicate(Op op, const Field *fvalue) : op(op), fvalue(fvalue) {}

const Field *IndexPredicate::getField() const {
    return fvalue;
}

Op IndexPredicate::getOp() const {
    return op;
}

bool IndexPredicate::operator==(const IndexPredicate &other) const {
    return op == other.op && *fvalue == *other.fvalue;
}

std::size_t std::hash<IndexPredicate>::operator()(const IndexPredicate &ipd) const {
    return int(ipd.getOp()) ^ std::hash<const Field*>()(ipd.getField());
}
