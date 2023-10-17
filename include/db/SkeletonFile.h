#ifndef DB_SKELETONFILE_H
#define DB_SKELETONFILE_H

#include <db/DbFile.h>

namespace db {
    class SkeletonPageId : public PageId {
    public:
        SkeletonPageId(int table, int page);

        int tableId;
        int pgNo;

        int getTableId() const override;

        int pageNumber() const override;

        bool operator==(const PageId &other) const override;
    };

    class SkeletonPage : public Page {
    public:
        explicit SkeletonPage(const PageId &pid);

        const PageId &pid;

        const PageId &getId() const override;

        void *getPageData() const override;
    };

    class SkeletonFileIterator {
    public:
        bool operator!=(const SkeletonFileIterator &other) const {
            return false;
        }

        SkeletonFileIterator &operator++() {
            return *this;
        }

        const Tuple *operator*() const {
            return {};
        }
    };

    class SkeletonFile : public DbFile {
        using iterator = SkeletonFileIterator;
        int id;
        TupleDesc td;
    public:
        int writes = 0;

        SkeletonFile(int id, const TupleDesc &td) : id(id), td(td) {}

        int getId() const override {
            return id;
        }

        const TupleDesc &getTupleDesc() const override {
            return td;
        }

        Page *readPage(const PageId &pid) override;

        void writePage(Page *p) override;

        iterator begin() const;

        iterator end() const;

        std::vector<Page *> insertTuple(TransactionId tid, Tuple &t) override;

        std::vector<Page *> deleteTuple(TransactionId tid, Tuple &t) override;
    };
}

#endif
