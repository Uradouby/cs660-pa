#include <db/Field.h>

using namespace db;

std::string db::to_string(Op op) {
    switch (op) {
        case Op::EQUALS:
            return "=";
        case Op::NOT_EQUALS:
            return "<>";
        case Op::GREATER_THAN:
            return ">";
        case Op::LESS_THAN:
            return "<";
        case Op::LESS_THAN_OR_EQ:
            return "<=";
        case Op::GREATER_THAN_OR_EQ:
            return ">=";
        case Op::LIKE:
            return "LIKE";
        default:
            return "UNKNOWN";
    }
}
