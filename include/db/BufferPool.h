#ifndef DB_BUFFERPOOL_H
#define DB_BUFFERPOOL_H

#include <db/Tuple.h>
#include <db/TransactionId.h>
#include <db/PagesMap.h>

/**
 * BufferPool manages the reading and writing of pages into memory from
 * disk. Access methods call into it to retrieve pages, and it fetches
 * pages from the appropriate location.
 * <p>
 * The BufferPool is also responsible for locking;  when a transaction fetches
 * a page, BufferPool checks that the transaction has the appropriate
 * locks to read/write the page.
 */
namespace db {
    class BufferPool {
        /** Default page size. Use the pageSize member instead. */
        static constexpr int PAGE_SIZE = 4096;
        /** Bytes per page, including header. */
        int pageSize = PAGE_SIZE;
        int numPages;
        PagesMap pages;

    public:
        BufferPool(const BufferPool &) = delete;

        const PagesMap &getPages() const;

        const int &getNumPages() const;

        /**
         * Discards a page from the buffer pool.
         * Flushes the page to disk to ensure dirty pages are updated on disk.
         */
        void evictPage();

        /**
         * Default number of pages passed to the constructor.
         * This is used by other classes.
         * BufferPool should use the numPages argument to the constructor instead.
         */
        static constexpr int DEFAULT_PAGES = 50;

        /**
         * Creates a BufferPool that caches up to numPages pages.
         * @param numPages maximum number of pages in this buffer pool.
         */
        explicit BufferPool(int numPages) : numPages(numPages) {}

        /**
         * Retrieve the specified page.
         * Will acquire a lock and may block if that lock is held by another
         * transaction.
         * <p>
         * The retrieved page should be looked up in the buffer pool.  If it
         * is present, it should be returned.  If it is not present, it should
         * be added to the buffer pool and returned.  If there is insufficient
         * space in the buffer pool, an page should be evicted and the new page
         * should be added in its place.
         *
         * @param pid the ID of the requested page
         */
        Page *getPage(const PageId *pid);

        int getPageSize() const { return pageSize; }

        /** DO NOT USE */
        void setPageSize(int newPageSize) { pageSize = newPageSize; }

        /** DO NOT USE */
        void resetPageSize() { setPageSize(PAGE_SIZE); }

        /**
         * Add a tuple to the specified table on behalf of transaction tid.  Will
         * acquire a write lock on the page the tuple is added to and any other
         * pages that are updated. May block if the lock(s) cannot be acquired.
         *
         * Marks any pages that were dirtied by the operation as dirty by calling
         * their markDirty bit, and adds versions of any pages that have
         * been dirtied to the cache (replacing any existing versions of those pages) so
         * that future requests see up-to-date pages.
         *
         * @param tid the transaction adding the tuple
         * @param tableId the table to add the tuple to
         * @param t the tuple to add
         */
        void insertTuple(const TransactionId &tid, int tableId, Tuple *t);

        /**
         * Remove the specified tuple from the buffer pool.
         * Will acquire a write lock on the page the tuple is removed from and any
         * other pages that are updated. May block if the lock(s) cannot be acquired.
         *
         * Marks any pages that were dirtied by the operation as dirty by calling
         * their markDirty bit, and adds versions of any pages that have
         * been dirtied to the cache (replacing any existing versions of those pages) so
         * that future requests see up-to-date pages.
         *
         * @param tid the transaction deleting the tuple.
         * @param t the tuple to delete
         */
        void deleteTuple(const TransactionId &tid, Tuple *t);

        /**
         * Flush all dirty pages to disk.
         */
        void flushAllPages();

        /** Remove the specific page id from the buffer pool.
         *  Needed by the recovery manager to ensure that the
         *  buffer pool doesn't keep a rolled back page in its
         *  cache.
         *
         *  Also used by B+ tree files to ensure that deleted pages
         *  are removed from the cache so they can be reused safely
         */
        void discardPage(const PageId *pid);

        /**
         * Flushes a certain page to disk, the page is not dirty anymore
         * @param pid an ID indicating the page to flush
         */
        void flushPage(const PageId *pid);

        /**
         * Write all pages of the specified transaction to disk.
         */
        void flushPages(const TransactionId &tid);
    };
}

#endif
