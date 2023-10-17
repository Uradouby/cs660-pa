#include <db/Predicate.h>

using namespace db;

// TODO pa2.2: implement
Predicate::Predicate(int field, Op op, const Field *operand) {}

int Predicate::getField() const {
    // TODO pa2.2: implement
    return {};
}

Op Predicate::getOp() const {
    // TODO pa2.2: implement
    return {};
}

const Field *Predicate::getOperand() const {
    // TODO pa2.2: implement
    return {};
}

bool Predicate::filter(const Tuple &t) const {
    // TODO pa2.2: implement
    return {};
}
