#ifndef DB_AGGREGATOR_H
#define DB_AGGREGATOR_H

#include <string>
#include <stdexcept>
#include <db/Tuple.h>
#include <db/DbIterator.h>

namespace db {

    /**
     * The common interface for any class that can compute an aggregate over a
     * list of Tuples.
     */
    class Aggregator {
    public:
        enum class Op {
            MIN, MAX, SUM, AVG, COUNT
        };
        static const int NO_GROUPING = -1;

        /**
         * Merge a new tuple into the aggregate for a distinct group value;
         * creates a new group aggregate result if the group value has not yet
         * been encountered.
         *
         * @param tup the Tuple containing an aggregate field and a group-by field
         */
        virtual void mergeTupleIntoGroup(Tuple *tup) = 0;

        /**
         * Create a DbIterator * over group aggregate results.
         */
        virtual DbIterator *iterator() const = 0;
    };

    std::string to_string(Aggregator::Op op);

}

#endif
