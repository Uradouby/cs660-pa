#include <db/Type.h>
#include <stdexcept>
#include <db/IntField.h>
#include <db/StringField.h>

using namespace db;

std::size_t Types::getLen(Type type) {
    switch (type) {
        case INT_TYPE:
            return sizeof(int);
        case STRING_TYPE:
            return STRING_LEN + sizeof(int);
        default:
            throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + " unexpected type");
    }
}

Field *Types::parse(uint8_t *data, Type type) {
    switch (type) {
        case INT_TYPE: {
            return IntField::parse(data);
        }
        case STRING_TYPE: {
            return StringField::parse(data);
        }
        default: {
            throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + " unexpected type");
        }
    }
}

std::string Types::to_string(db::Types::Type type) {
    switch (type) {
        case INT_TYPE:
            return "INT";
        case STRING_TYPE:
            return "STRING";
        default:
            throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + " unexpected type");
    }
}
