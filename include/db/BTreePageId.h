#ifndef DB_BTREE_PAGEID_H
#define DB_BTREE_PAGEID_H

#include <db/PageId.h>

namespace db {
    enum class BTreePageType {
        ROOT_PTR, INTERNAL, LEAF, HEADER
    };

    /**
     * Unique identifier for BTreeInternalPage, BTreeLeafPage, BTreeHeaderPage
     *  and BTreeRootPtrPage objects.
     */
    class BTreePageId : public PageId {
        const int tableId;
        const int pgNo;
        BTreePageType type;
    public:
        /**
         * Constructor. Create a page id structure for a specific page of a
         * specific table.
         *
         * @param tableId The table that is being referenced
         * @param pgNo The page number in that table.
         * @param type which kind of page it is
         */
        BTreePageId(int tableId, int pgNo, BTreePageType type) : tableId(tableId), pgNo(pgNo), type(type) {}

        /** @return the table associated with this PageId */
        int getTableId() const override { return tableId; }

        /**
         * @return the page number in the table getTableId() associated with
         *   this PageId
         */
        int pageNumber() const override {
            return pgNo;
        }

        /**
         * @return the category of this page
         */
        BTreePageType getType() const { return type; }

        bool operator==(const PageId &other) const override;
    };
}

template<>
struct std::hash<db::BTreePageId> {
    size_t operator()(const db::BTreePageId &pid) const;
};

#endif
