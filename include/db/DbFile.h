#ifndef DB_DBFILE_H
#define DB_DBFILE_H

#include <db/TransactionId.h>
#include <db/TupleDesc.h>
#include <db/Tuple.h>
#include <db/PageId.h>
#include <db/Page.h>
#include <vector>

namespace db {
    /**
     * The interface for database files on disk. Each table is represented by a
     * single DbFile. DbFiles can fetch pages and iterate through tuples. Each
     * file has a unique id used to store metadata about the table in the Catalog.
     * DbFiles are generally accessed through the buffer pool, rather than directly
     * by operators.
     */
    class DbFile {
    public:
        /**
         * Read the specified page from disk.
         */
        virtual Page *readPage(const PageId &id) = 0;

        /**
         * Push the specified page to disk.
         *
         * @param p The page to write.  page.getId().pageno() specifies the offset into the file where the page should be written.
         *
         */
        virtual void writePage(Page *p) = 0;

        /**
         * Inserts the specified tuple to the file on behalf of transaction.
         * This method will acquire a lock on the affected pages of the file, and
         * may block until the lock can be acquired.
         *
         * @param tid The transaction performing the update
         * @param t The tuple to add.  This tuple should be updated to reflect that
         *          it is now stored in this file.
         * @return A vector contain the pages that were modified
         */
        virtual std::vector<Page *> insertTuple(TransactionId tid, Tuple &t) = 0;

        /**
         * Removes the specified tuple from the file on behalf of the specified
         * transaction.
         * This method will acquire a lock on the affected pages of the file, and
         * may block until the lock can be acquired.
         *
         * @param tid The transaction performing the update
         * @param t The tuple to delete.  This tuple should be updated to reflect that
         *          it is no longer stored on any page.
         * @return A vector contain the pages that were modified
         */
        virtual std::vector<Page *> deleteTuple(TransactionId tid, Tuple &t) = 0;

        /**
         * Returns a unique ID used to identify this DbFile in the Catalog. This id
         * can be used to look up the table via {@link Catalog#getDatabaseFile} and
         * {@link Catalog#getTupleDesc}.
         * <p>
         * Implementation note:  you will need to generate this tableid somewhere,
         * ensure that each HeapFile has a "unique id," and that you always
         * return the same value for a particular HeapFile. A simple implementation
         * is to use the hash code of the absolute path of the file underlying
         * the HeapFile, i.e. <code>f.getAbsoluteFile().hashCode()</code>.
         *
         * @return an ID uniquely identifying this HeapFile.
         */
        virtual int getId() const = 0;

        /**
         * Returns the TupleDesc of the table stored in this DbFile.
         * @return TupleDesc of this DbFile.
         */
        virtual const TupleDesc &getTupleDesc() const = 0;

        virtual ~DbFile() = default;
    };
}

#endif
