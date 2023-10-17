#ifndef DB_BTREE_PAGE_H
#define DB_BTREE_PAGE_H

#include <db/TupleDesc.h>
#include <db/Page.h>
#include <db/BTreePageId.h>

namespace db {
    /**
     * Each instance of BTreeInternalPage stores data for one page of a BTreeFile and
     * implements the Page interface that is used by BufferPool.
     *
     * @see BTreeFile
     * @see BufferPool
     *
     */
    class BTreePage : public Page {
    protected:
        BTreePageId pid;

        const TupleDesc td;
        const int keyField;

        int parent; // parent is always internal node or 0 for root node

    public:

        /**
         * Create a BTreeInternalPage from a set of bytes of data read from disk.
         * The format of a BTreeInternalPage is a set of header bytes indicating
         * the slots of the page that are in use, some number of entry slots, and extra
         * bytes for the parent pointer, one extra child pointer (a node with m entries
         * has m+1 pointers to children), and the category of all child pages (either
         * leaf or internal).
         *  Specifically, the number of entries is equal to: <p>
         *          floor((BufferPool.getPageSize()*8 - extra bytes*8) / (entry size * 8 + 1))
         * <p> where entry size is the size of entries in this index node
         * (key + child pointer), which can be determined via the key field and
         * {@link Catalog#getTupleDesc}.
         * The number of 8-bit header words is equal to:
         * <p>
         *      ceiling((no. entry slots + 1) / 8)
         * <p>
         * @see Database#getCatalog
         * @see Catalog#getTupleDesc
         * @see BufferPool#getPageSize()
         *
         * @param id - the id of this page
         * @param data - the raw data of this page
         * @param key - the field which the index is keyed on
         */
        BTreePage(const BTreePageId &id, int key);


        /**
         * @return the PageId associated with this page.
         */
        const BTreePageId &getId() const override;

        /**
         * Static method to generate a byte array corresponding to an empty
         * BTreePage.
         * Used to add new, empty pages to the file. Passing the results of
         * this method to the BTreeInternalPage or BTreeLeafPage constructor will create a BTreePage with
         * no valid entries in it.
         *
         * @return The returned ByteArray.
         */
        static void *createEmptyPageData();

        /**
         * Get the parent id of this page
         * @return the parent id
         */
        BTreePageId *getParentId();

        /**
         * Set the parent id
         * @param id - the id of the parent of this page
         */
        void setParentId(const BTreePageId *id);

        /**
         * Returns the number of empty slots on this page.
         */
        virtual int getNumEmptySlots() const = 0;

        /**
         * Returns true if associated slot on this page is filled.
         */
        virtual bool isSlotUsed(int i) const = 0;
    };
}

#endif
