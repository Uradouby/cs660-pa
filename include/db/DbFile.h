#ifndef DB_DBFILE_H
#define DB_DBFILE_H

#include <db/TupleDesc.h>
#include <db/Tuple.h>
#include <db/TransactionId.h>
#include <db/Page.h>

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
