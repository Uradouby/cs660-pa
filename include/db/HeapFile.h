#ifndef DB_HEAPFILE_H
#define DB_HEAPFILE_H

#include <db/TupleDesc.h>
#include <db/Tuple.h>
#include <db/Page.h>
#include <db/PageId.h>
#include <db/DbFile.h>
#include <db/HeapPage.h>
#include <db/HeapPageId.h>

namespace db {
    class HeapFileIterator {
        int numPages;
        HeapPageId hpid;
        bool end;
        HeapPageIterator *it;
        HeapPage *page;

    public:

        HeapFileIterator(int tableid, int numPages, bool end = false);

        bool operator!=(const HeapFileIterator &other) const;

        Tuple &operator*() const;

        HeapFileIterator &operator++();
    };

    /**
     * HeapFile is an implementation of a DbFile that stores a collection of tuples
     * in no particular order. Tuples are stored on pages, each of which is a fixed
     * size, and the file is simply a collection of those pages. HeapFile works
     * closely with HeapPage. The format of HeapPages is described in the HeapPage
     * constructor.
     *
     * @see db::HeapPage::HeapPage
     * @author Sam Madden
     */
    class HeapFile : public DbFile {
        int fd;
        int tableid;
        const TupleDesc &td;
        int numPages;
    public:

        /**
         * Constructs a heap file backed by the specified file.
         *
         * @param f the file that stores the on-disk backing store for this heap file.
         */
        HeapFile(const char *fname, const TupleDesc &td);

        /**
         * Returns an ID uniquely identifying this HeapFile. Implementation note:
         * you will need to generate this tableid somewhere ensure that each
         * HeapFile has a "unique id," and that you always return the same value for
         * a particular HeapFile. We suggest hashing the absolute file name of the
         * file underlying the heapfile, i.e. f.getAbsoluteFile().hashCode().
         * @return an ID uniquely identifying this HeapFile.
         */
        int getId() const override;

        /**
         * Returns the TupleDesc of the table stored in this DbFile.
         * @return TupleDesc of this DbFile.
         */
        const TupleDesc &getTupleDesc() const override;

        Page *readPage(const PageId &pid) override;

        std::vector<Page *> insertTuple(TransactionId tid, Tuple &t) override;

        std::vector<Page *> deleteTuple(TransactionId tid, Tuple &t) override;

        /**
         * Returns the number of pages in this HeapFile.
         */
        int getNumPages() const;

        HeapFileIterator begin() const;

        HeapFileIterator end() const;

        void writePage(Page *p) override;
    };
}

#endif
