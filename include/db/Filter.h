#ifndef DB_FILTER_H
#define DB_FILTER_H

#include <db/Predicate.h>
#include <db/Operator.h>

namespace db {
/**
 * Filter is an operator that implements a relational select.
 */
    class Filter : public Operator {
        // TODO pa3.1: add private members
    protected:
        /**
         * AbstractDbIterator *.readNext implementation. Iterates over tuples from the
         * child operator, applying the predicate to them and returning those that
         * pass the predicate (i.e. for which the Predicate.filter() returns true.)
         *
         * @return The next tuple that passes the filter, or null if there are no
         *         more tuples
         * @see Predicate#filter
         */
        std::optional<Tuple> fetchNext() override;

    public:
        /**
         * Constructor accepts a predicate to apply and a child operator to read
         * tuples to filter from.
         *
         * @param p
         *            The predicate to filter tuples with
         * @param child
         *            The child operator
         */
        Filter(Predicate p, DbIterator *child);

        Predicate *getPredicate();

        const TupleDesc &getTupleDesc() const override;

        void open() override;

        void close() override;

        void rewind() override;

        std::vector<DbIterator *> getChildren() override;

        void setChildren(std::vector<DbIterator *> children) override;

    };
}
#endif
