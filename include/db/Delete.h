#ifndef DB_DELETE_H
#define DB_DELETE_H

#include <db/TransactionId.h>
#include <db/TupleDesc.h>
#include <db/Tuple.h>
#include <db/Operator.h>
#include <db/DbIterator.h>

namespace db {
    /**
     * The delete operator. Delete reads tuples from its child operator and removes
     * them from the table they belong to.
     */
    class Delete : public Operator {
        // TODO pa3.4: add private members
    protected:
        /**
         * Deletes tuples as they are read from the child operator. Deletes are
         * processed via the buffer pool (which can be accessed via the
         * Database.getBufferPool() method.
         *
         * @return A 1-field tuple containing the number of deleted records.
         * @see Database#getBufferPool
         * @see BufferPool#deleteTuple
         */
        std::optional<Tuple> fetchNext() override;

    public:
        /**
         * Constructor specifying the transaction that this delete belongs to as
         * well as the child to read from.
         *
         * @param t
         *            The transaction this delete runs in
         * @param child
         *            The child operator from which to read tuples for deletion
         */
        Delete(TransactionId t, DbIterator *child);

        const TupleDesc &getTupleDesc() const override;

        void open() override;

        void close() override;

        void rewind() override;

        std::vector<DbIterator *> getChildren() override;

        void setChildren(std::vector<DbIterator *> children) override;

    };
}

#endif
