#ifndef DB_INTFIELD_H
#define DB_INTFIELD_H

#include <db/Field.h>
#include <cstring>

namespace db {
    /**
     * Instance of Field that stores a single integer.
     */
    class IntField : public Field {
        int value;
    public:
        /**
         * @param value The value of this field.
         */
        explicit IntField(int value);

        int getValue() const;

        bool operator==(const Field &other) const override;

        Types::Type getType() const override;

        void serialize(void *data) const override;

        static Field *parse(void *data);

        std::string to_string() const override {
            return std::to_string(value);
        }
    };
}

#endif
