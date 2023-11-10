#ifndef DB_INSERT_H
#define DB_INSERT_H

#include <db/Operator.h>
#include <db/TransactionId.h>

namespace db {
/**
 * Inserts tuples read from the child operator into the tableId specified in the
 * constructor
 */
    class Insert : public Operator {
        // TODO pa3.3: add private members
    protected:
        /**
         * Inserts tuples read from child into the tableId specified by the
         * constructor. It returns a one field tuple containing the number of
         * inserted records. Inserts should be passed through BufferPool. An
         * instances of BufferPool is available via Database.getBufferPool(). Note
         * that insert DOES NOT need check to see if a particular tuple is a
         * duplicate before inserting it.
         *
         * @return A 1-field tuple containing the number of inserted records, or
         *         null if called more than once.
         * @see Database#getBufferPool
         * @see BufferPool#insertTuple
         */
        std::optional<Tuple> fetchNext() override;

    public:
        /**
         * Constructor.
         *
         * @param t
         *            The transaction running the insert.
         * @param child
         *            The child operator from which to read tuples to be inserted.
         * @param tableId
         *            The table in which to insert tuples.
         */
        Insert(TransactionId t, DbIterator *child, int tableId);

        const TupleDesc &getTupleDesc() const override;

        void open() override;

        void close() override;

        void rewind() override;

        std::vector<DbIterator *> getChildren() override;

        void setChildren(std::vector<DbIterator *> children) override;
    };
}
#endif
