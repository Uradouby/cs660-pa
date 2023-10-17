#ifndef DB_STRINGFIELD_H
#define DB_STRINGFIELD_H

#include <db/Field.h>
#include <cstring>

namespace db {
    /**
     * Instance of Field that stores a single integer.
     */
    class StringField : public Field {
        int len;
        char value[Types::STRING_LEN];
    public:
        /**
         * @param value The value of this field.
         */
        explicit StringField(const char* value);

        std::string getValue() const;

        bool operator==(const Field &other) const override;

        Types::Type getType() const override;

        void serialize(void *data) const override;

        static Field *parse(void *data);

        std::string to_string() const override {
            return value;
        }

        bool compare(Op op, const Field *value) const override;
    };
}

#endif
