#include <db/Predicate.h>

using namespace db;

Predicate::Predicate(int field, Op op, const Field *operand) : field(field), op(op), operrand(operand) {}

int Predicate::getField() const {
    return field;
}

Op Predicate::getOp() const {
    return op;
}

const Field *Predicate::getOperand() const {
    return operrand;
}

bool Predicate::filter(const Tuple &t) const {
    return t.getField(field).compare(op, operrand);
}
