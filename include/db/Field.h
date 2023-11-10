#ifndef DB_FIELD_H
#define DB_FIELD_H

#include <ostream>
#include <db/Type.h>
#include <db/Predicate.h>

namespace db {
    /**
     * Interface for values of fields in tuples.
     */
    class Field {
    public:
        virtual ~Field() = default;

        virtual bool operator==(const Field &other) const = 0;

        bool operator!=(const Field &other) const { return !(*this == other); }

        virtual Types::Type getType() const = 0;

        virtual void serialize(void *data) const = 0;

        virtual std::string to_string() const = 0;

        /**
         * Compare the value of this field object to the passed in value.
         * @param op The operator
         * @param value The value to compare this Field to
         * @return Whether or not the comparison yields true.
         */
        virtual bool compare(Predicate::Op op, const Field *value) const = 0;
    };
}

#endif
