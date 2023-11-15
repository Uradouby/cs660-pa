#include <db/Predicate.h>
#include <db/Tuple.h>

using namespace db;

Predicate::Predicate(int field, Op op, const Field *operand) : field(field), op(op), operand(operand) {}

int Predicate::getField() const {
    return field;
}

Predicate::Op Predicate::getOp() const {
    return op;
}

const Field *Predicate::getOperand() const {
    return operand;
}

bool Predicate::filter(const Tuple &t) const {
    return t.getField(field).compare(op, operand);
}

std::string Predicate::to_string() const {
    return "f = " + std::to_string(getField()) + " op = " + ::to_string(getOp()) +
           " operand = " + getOperand()->to_string();
}
