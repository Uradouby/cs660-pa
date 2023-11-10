#ifndef DB_OPERATOR_H
#define DB_OPERATOR_H

#include <stdexcept>
#include <db/Tuple.h>
#include <db/DbIterator.h>
#include <optional>

namespace db {

/**
 * Abstract class for implementing operators. It handles <code>close</code>,
 * <code>next</code> and <code>hasNext</code>. Subclasses only need to implement
 * <code>begin</code> and <code>readNext</code>.
 */
    class Operator : public DbIterator {
        std::optional<Tuple> tup = std::nullopt;
        bool isOpen = false;
        int estimatedCardinality = 0;
    protected:
        /**
         * Returns the next Tuple in the iterator, or null if the iteration is
         * finished. Operator uses this method to implement both <code>next</code>
         * and <code>hasNext</code>.
         *
         * @return the next Tuple in the iterator, or null if the iteration is
         *         finished.
         */
        virtual std::optional<Tuple> fetchNext() = 0;

        /**
         * @param card
         *            The estimated cardinality of this operator
         */
        void setEstimatedCardinality(int card);

    public:
        bool hasNext() override;

        Tuple next() override;

        /**
         * Closes this iterator. If overridden by a subclass, they should call
         * super.close() in order for Operator's internal state to be consistent.
         */
        virtual void close() override;

        void open() override;

        void rewind() override;

        /**
         * @return return the children DbIterator *s of this operator. If there is
         *         only one child, return an array of only one element. For join
         *         operators, the order of the children is not important. But they
         *         should be consistent among multiple calls.
         */
        virtual std::vector<DbIterator *> getChildren() = 0;

        /**
         * Set the children(child) of this operator. If the operator has only one
         * child, children[0] should be used. If the operator is a join, children[0]
         * and children[1] should be used.
         *
         *
         * @param children
         *            the DbIterator *s which are to be set as the children(child) of
         *            this operator
         */
        virtual void setChildren(std::vector<DbIterator *> children) = 0;

        /**
         * @return return the TupleDesc of the output tuples of this operator
         */
        virtual const TupleDesc &getTupleDesc() const override = 0;

        /**
         * @return The estimated cardinality of this operator.
         */
        int getEstimatedCardinality() const;

    };
}
#endif