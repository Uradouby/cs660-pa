#include <db/Field.h>

using namespace db;

std::string db::to_string(Predicate::Op op) {
    switch (op) {
        case Predicate::Op::EQUALS:
            return "=";
        case Predicate::Op::NOT_EQUALS:
            return "<>";
        case Predicate::Op::GREATER_THAN:
            return ">";
        case Predicate::Op::LESS_THAN:
            return "<";
        case Predicate::Op::LESS_THAN_OR_EQ:
            return "<=";
        case Predicate::Op::GREATER_THAN_OR_EQ:
            return ">=";
        case Predicate::Op::LIKE:
            return "LIKE";
        default:
            return "UNKNOWN";
    }
}
