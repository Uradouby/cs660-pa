#include <db/IntField.h>

using namespace db;

IntField::IntField(int value) {
    this->value = value;
}

int IntField::getValue() const {
    return value;
}

bool IntField::operator==(const Field &other) const {
    if (auto otherIntField = dynamic_cast<const IntField *>(&other)) {
        return value == otherIntField->value;
    }
    return false;
}

Types::Type IntField::getType() const {
    return Types::Type::INT_TYPE;
}

void IntField::serialize(void *data) const {
    memcpy(data, &value, sizeof(int));
}

Field *IntField::parse(void *data) {
    int value;
    memcpy(&value, data, sizeof(int));
    return new IntField(value);
}

bool IntField::compare(Op op, const Field *val) const {
    const IntField *iVal = dynamic_cast<const IntField *>(val);
    switch (op) {
        case Op::EQUALS:
            return value == iVal->value;
        case Op::NOT_EQUALS:
            return value != iVal->value;
        case Op::GREATER_THAN:
            return value > iVal->value;
        case Op::GREATER_THAN_OR_EQ:
            return value >= iVal->value;
        case Op::LESS_THAN:
            return value < iVal->value;
        case Op::LESS_THAN_OR_EQ:
            return value <= iVal->value;
        case Op::LIKE:
            return value == iVal->value;
    }
    return false;
}
