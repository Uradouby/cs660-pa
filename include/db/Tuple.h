#ifndef DB_TUPLE_H
#define DB_TUPLE_H

#include <db/TupleDesc.h>
#include <db/Field.h>
#include <db/RecordId.h>

namespace db {
    /**
     * Tuple maintains information about the contents of a tuple.
     * Tuples have a specified schema specified by a TupleDesc object
     * and contain Field objects with the data for each field.
     */
    class Tuple {
        using iterator = std::vector<const Field*>::const_iterator;
        const RecordId *rid;
        std::vector<const Field*> fields;
        TupleDesc td;
    public:
        Tuple() = default;

        /**
         * Create a new tuple with the specified schema (type).
         * @param td the schema of this tuple. It must be a valid TupleDesc instance with at least one field.
         */
        explicit Tuple(const TupleDesc &td, RecordId *rid = nullptr);

        /**
         * @return The TupleDesc representing the schema of this tuple.
         */
        const TupleDesc &getTupleDesc() const;

        /**
         * @return The RecordId representing the location of this tuple on disk. May be null.
         */
        const RecordId *getRecordId() const;

        /**
         * Set the RecordId information for this tuple.
         * @param rid the new RecordId for this tuple.
         */
        void setRecordId(const RecordId *id);

        /**
         * @return the value of the ith field, or null if it has not been set.
         * @param i field index to return. Must be a valid index.
         */
        const Field &getField(int i) const;

        /**
         * Change the value of the ith field of this tuple.
         * @param i index of the field to change. It must be a valid index.
         * @param f new value for the field.
         */
        void setField(int i, const Field *f);

        /**
         *   An iterator which iterates over all the fields of a tuple
         */
        iterator begin() const;

        iterator end() const;

        std::string to_string() const;
    };

}

#endif
