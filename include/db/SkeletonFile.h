#ifndef DB_SKELETONFILE_H
#define DB_SKELETONFILE_H

namespace db {
    class SkeletonFileIterator {
    public:
        bool operator!=(const SkeletonFileIterator &other) const {
            return false;
        }

        SkeletonFileIterator &operator++() {
            return *this;
        }

        const Tuple &operator*() const {
            return {};
        }
    };

    class SkeletonFile : public DbFile {
        using iterator = SkeletonFileIterator;
        int id;
        TupleDesc td;
    public:
        SkeletonFile(int id, TupleDesc td) : id(id), td(td) {}

        int getId() const override {
            return id;
        }

        const TupleDesc &getTupleDesc() const override {
            return td;
        }

        Page *readPage(const PageId &pid) override;

        iterator begin() const;

        iterator end() const;
    };
}

#endif
