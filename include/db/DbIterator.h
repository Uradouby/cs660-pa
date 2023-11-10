#ifndef DB_DbIterator_H
#define DB_DbIterator_H

namespace db {
    /**
     * DbIterator is the iterator interface that all operators should implement.
     */
    class DbIterator {

    public:
        /**
         * Opens the iterator. This must be called before any of the other methods.
         */
        virtual void open() = 0;

        /** Returns true if the iterator has more tuples.
         * @return true f the iterator has more tuples.
       */
        virtual bool hasNext() = 0;

        /**
         * Returns the next tuple from the operator (typically implementing by reading
         * from a child operator or an access method).
         *
         * @return the next tuple in the iteration.
         */
        virtual Tuple next() = 0;

        /**
         * Resets the iterator to the start.
         */
        virtual void rewind() = 0;

        /**
         * Returns the TupleDesc associated with this DbIterator *.
         * @return the TupleDesc associated with this DbIterator *.
         */
        virtual const TupleDesc &getTupleDesc() const = 0;

        /**
         * Closes the iterator. When the iterator is closed, calling next(),
         * hasNext(), or rewind() should fail by throwing IllegalStateException.
         */
        virtual void close() = 0;
    };
}

#endif
