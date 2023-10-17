#ifndef DB_INDEX_PREDICATE_H
#define DB_INDEX_PREDICATE_H

#include <db/Predicate.h>
#include <db/Field.h>

namespace db {
/**
 * IndexPredicate compares a field which has index on it against a given value
 * @see simpledb.IndexDbIterator
 */
    class IndexPredicate {
        // TODO pa2.2: add private members

        /**
         * Constructor.
         *
         * @param fvalue The value that the predicate compares against.
         * @param op The operation to apply (as defined in Predicate.Op); either
         *   Predicate.Op.GREATER_THAN, Predicate.Op.LESS_THAN, Predicate.Op.EQUAL,
         *   Predicate.Op.GREATER_THAN_OR_EQ, or Predicate.Op.LESS_THAN_OR_EQ
         * @see Predicate
         */
    public:
        IndexPredicate(Op op, const Field *fvalue);

        const Field *getField() const;

        Op getOp() const;

        /** Return true if the fieldvalue in the supplied predicate
            is satisfied by this predicate's fieldvalue and
            operator.
            @param ipd The field to compare against.
        */
        bool operator==(const IndexPredicate &other) const;
    };
}

template<>
struct std::hash<db::IndexPredicate> {
    std::size_t operator()(const db::IndexPredicate &ipd) const;
};

#endif
