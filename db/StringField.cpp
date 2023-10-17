#include <db/StringField.h>

using namespace db;

StringField::StringField(const char *str) {
    len = std::min(strlen(str), Types::STRING_LEN - 1);
    strncpy(value, str, len);
    value[len] = '\0';
}

std::string StringField::getValue() const {
    return value;
}

bool StringField::operator==(const Field &other) const {
    if (auto otherStringField = dynamic_cast<const StringField *>(&other)) {
        return value == otherStringField->value;
    }
    return false;
}

Types::Type StringField::getType() const {
    return Types::Type::STRING_TYPE;
}

void StringField::serialize(void *data) const {
    auto *ptr = (uint8_t *) data;
    memcpy(ptr, &len, sizeof(int));
    memcpy(ptr + sizeof(int), &value, sizeof(int));
}

Field *StringField::parse(void *data) {
    auto *ptr = (uint8_t *) data;
    int len;
    memcpy(&len, ptr, sizeof(int));
    char value[Types::STRING_LEN];
    memcpy(value, ptr + sizeof(int), len);
    return new StringField(value);
}

bool StringField::compare(Op op, const Field *value) const {
    throw std::runtime_error("Not implemented");
}
