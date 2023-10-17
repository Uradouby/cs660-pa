#ifndef DB_BTREE_FILE_H
#define DB_BTREE_FILE_H

#include <db/BTreeRootPtrPage.h>
#include <db/DbFile.h>
#include <db/IndexPredicate.h>
#include <db/TransactionId.h>
#include <db/BTreeInternalPage.h>
#include <db/TupleDesc.h>
#include <db/PagesMap.h>
#include <db/BTreeLeafPage.h>

namespace db {
    class BTreeFile;

    class BTreeFileIterator {
        TransactionId tid;
        IndexPredicate *pred;
        BTreeFile *file;

        BTreeLeafPage *current_leaf;
        BTreeLeafPageIterator it;
    public:
        BTreeFileIterator(TransactionId tid, IndexPredicate *pred, BTreeFile *file, bool done=false);

        bool operator!=(const BTreeFileIterator &other) const;

        BTreeFileIterator &operator++();

        Tuple &operator*() const;
    };

    class BTreeIterable {
        TransactionId id;
        IndexPredicate *pPredicate;
        BTreeFile *pFile;
    public:
        BTreeIterable(TransactionId id, IndexPredicate *pPredicate, db::BTreeFile *pFile) : id(id), pPredicate(pPredicate), pFile(pFile) {}

        BTreeFileIterator begin();

        BTreeFileIterator end();
    };
    /**
     * BTreeFile is an implementation of a DbFile that stores a B+ tree.
     * Specifically, it stores a pointer to a root page,
     * a set of internal pages, and a set of leaf pages, which contain a collection of tuples
     * in sorted order. BTreeFile works closely with BTreeLeafPage, BTreeInternalPage,
     * and BTreeRootPtrPage. The format of these pages is described in their constructors.
     *
     * @see BTreeLeafPage::BTreeLeafPage
     * @see BTreeInternalPage::BTreeInternalPage
     * @see BTreeHeaderPage::BTreeHeaderPage
     * @see BTreeRootPtrPage::BTreeRootPtrPage
     * @author Becca Taft
     */
    class BTreeFile : public DbFile {
        friend class BTreeLeafPage;

        const TupleDesc &td;
        int tableid;
        int fd;
        const int keyField;

        /**
         * Method to encapsulate the process of getting a parent page ready to accept new entries.
         * This may mean creating a page to become the new root of the tree, splitting the existing 
         * parent page if there are no empty slots, or simply locking and returning the existing parent page.
         *
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param parentId - the id of the parent. May be an internal page or the RootPtr page
         * @param field - the key of the entry which will be inserted. Needed in case the parent must be split
         * to accommodate the new entry
         * @return the parent page, guaranteed to have at least one empty slot
         * @see splitInternalPage
         */
        BTreeInternalPage *getParentWithEmptySlots(TransactionId tid,
                                                   PagesMap &dirtypages,
                                                   BTreePageId *parentId, Field *field);

        /**
         * Function which finds and locks the leaf page in the B+ tree corresponding to
         * the left-most page possibly containing the key field f. It locks all internal
         * nodes along the path to the leaf node with READ_ONLY permission, and locks the 
         * leaf node with permission perm.
         * 
         * If f is null, it finds the left-most leaf page -- used for the iterator
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param pid - the current page being searched
         * @param perm - the permissions with which to lock the leaf page
         * @param f - the field to search for
         * @return the left-most leaf page possibly containing the key field f
         * 
         */
        BTreeLeafPage *findLeafPage(TransactionId tid, PagesMap &dirtypages,
                                    BTreePageId *pid, Permissions perm, const Field *f);

        /**
         * Helper function to update the parent pointer of a node.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param pid - id of the parent node
         * @param child - id of the child node to be updated with the parent pointer
         */
        void updateParentPointer(TransactionId tid, PagesMap &dirtypages,
                                 const BTreePageId *pid, BTreePageId *child);

        /**
         * Update the parent pointer of every child of the given page so that it correctly points to
         * the parent
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param page - the parent page
         * @see updateParentPointer
         */
        void updateParentPointers(TransactionId tid, PagesMap &dirtypages,
                                  BTreeInternalPage *page);

        /**
         * Handle the case when a B+ tree page becomes less than half full due to deletions.
         * If one of its siblings has extra tuples/entries, redistribute those tuples/entries.
         * Otherwise merge with one of the siblings. Update pointers as needed.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param page - the page which is less than half full
         * @see handleMinOccupancyLeafPage
         * @see handleMinOccupancyInternalPage
         */
        void handleMinOccupancyPage(TransactionId tid, PagesMap &dirtypages,
                                    BTreePage *page);

        /**
         * Handle the case when a leaf page becomes less than half full due to deletions.
         * If one of its siblings has extra tuples, redistribute those tuples.
         * Otherwise merge with one of the siblings. Update pointers as needed.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param page - the leaf page which is less than half full
         * @param parent - the parent of the leaf page
         * @param leftEntry - the entry in the parent pointing to the given page and its left-sibling
         * @param rightEntry - the entry in the parent pointing to the given page and its right-sibling
         * @see mergeLeafPages
         * @see stealFromLeafPage
         */
        void handleMinOccupancyLeafPage(TransactionId tid, PagesMap &dirtypages,
                                        BTreeLeafPage *page, BTreeInternalPage *parent, BTreeEntry *leftEntry,
                                        BTreeEntry *rightEntry);

        /**
         * Handle the case when an internal page becomes less than half full due to deletions.
         * If one of its siblings has extra entries, redistribute those entries.
         * Otherwise merge with one of the siblings. Update pointers as needed.
         *
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param page - the internal page which is less than half full
         * @param parent - the parent of the internal page
         * @param leftEntry - the entry in the parent pointing to the given page and its left-sibling
         * @param rightEntry - the entry in the parent pointing to the given page and its right-sibling
         * @see mergeInternalPages
         * @see stealFromLeftInternalPage
         * @see stealFromRightInternalPage
         */
        void handleMinOccupancyInternalPage(TransactionId tid, PagesMap &dirtypages,
                                            BTreeInternalPage *page, BTreeInternalPage *parent, BTreeEntry *leftEntry,
                                            BTreeEntry *rightEntry);

        /**
         * Method to encapsulate the process of deleting an entry (specifically the key and right child) 
         * from a parent node.  If the parent becomes empty (no keys remaining), that indicates that it 
         * was the root node and should be replaced by its one remaining child.  Otherwise, if it gets 
         * below minimum occupancy for non-root internal nodes, it should steal from one of its siblings or 
         * merge with a sibling.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param leftPage - the child remaining after the key and right child are deleted
         * @param parent - the parent containing the entry to be deleted
         * @param parentEntry - the entry to be deleted
         * @see handleMinOccupancyPage
         */
        void deleteParentEntry(TransactionId tid, PagesMap &dirtypages,
                               BTreePage *leftPage, BTreeInternalPage *parent, BTreeEntry *parentEntry);


        /**
         * Method to encapsulate the process of creating a new page.  It reuses old pages if possible,
         * and creates a new page if none are available.  It wipes the page on disk and in the cache and 
         * returns a clean copy locked with read-write permission
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param type - the BTreePageId category of the new page.  Either LEAF, INTERNAL, or HEADER
         * @return the new empty page
         * @see getEmptyPageNo
         * @see setEmptyPage
         */
        Page *getEmptyPage(TransactionId tid, PagesMap &dirtypages, BTreePageType type);

    protected:
        /**
         * Split a leaf page to make room for new tuples and split the parent node as needed to accommodate a new entry.
         * The new entry should have a key matching the key field of the first tuple in the right-hand page
         * (the key is "copied up"), and child pointers pointing to the two leaf pages resulting from the split.
         * Update sibling pointers and parent pointers as needed.
         *
         * Return the leaf page into which a new tuple with key field "field" should be inserted.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param page - the leaf page to split
         * @param field - the key field of the tuple to be inserted after the split is complete. Necessary to know
         * which of the two pages to return.
         * @see getParentWithEmptySlots
         *
         * @return the leaf page into which the new tuple should be inserted
         */
        BTreeLeafPage *splitLeafPage(TransactionId tid, PagesMap &dirtypages,
                                     BTreeLeafPage *page, const Field *field);

        /**
         * Split an internal page to make room for new entries and split its parent page as needed to accommodate
         * a new entry. The new entry for the parent should have a key matching
         * the middle key in the original internal page being split (this key is "pushed up" to the parent). 
         * The child pointers of the new parent entry should point to the two internal pages resulting 
         * from the split. Update parent pointers as needed.
         * 
         * Return the internal page into which an entry with key field "field" should be inserted
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param page - the internal page to split
         * @param field - the key field of the entry to be inserted after the split is complete. Necessary to know
         * which of the two pages to return.
         * @see getParentWithEmptySlots
         * @see updateParentPointers
         *
         * @return the internal page into which the new entry should be inserted
         */
        BTreeInternalPage *splitInternalPage(TransactionId tid, PagesMap &dirtypages,
                                             BTreeInternalPage *page, Field *field);

        /**
         * Steal tuples from a sibling and copy them to the given page so that both pages are at least
         * half full.  Update the parent's entry so that the key matches the key field of the first
         * tuple in the right-hand page.
         * 
         * @param page - the leaf page which is less than half full
         * @param sibling - the sibling which has tuples to spare
         * @param parent - the parent of the two leaf pages
         * @param entry - the entry in the parent pointing to the two leaf pages
         * @param isRightSibling - whether the sibling is a right-sibling
         */
        void stealFromLeafPage(BTreeLeafPage *page, BTreeLeafPage *sibling, BTreeInternalPage *parent,
                               BTreeEntry *entry, bool isRightSibling);

        /**
         * Steal entries from the right sibling and copy them to the given page so that both pages are at least
         * half full. Keys can be thought of as rotating through the parent entry, so the original key in the 
         * parent is "pulled down" to the left-hand page, and the last key in the right-hand page is "pushed up"
         * to the parent.  Update parent pointers as needed.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param page - the internal page which is less than half full
         * @param rightSibling - the right sibling which has entries to spare
         * @param parent - the parent of the two internal pages
         * @param parentEntry - the entry in the parent pointing to the two internal pages
         * @see updateParentPointers
         */
        void stealFromRightInternalPage(TransactionId tid, PagesMap &dirtypages,
                                        BTreeInternalPage *page, BTreeInternalPage *rightSibling,
                                        BTreeInternalPage *parent, BTreeEntry *parentEntry);

        /**
         * Merge two leaf pages by moving all tuples from the right page to the left page. 
         * Delete the corresponding key and right child pointer from the parent, and
         * handle the case when the parent gets below minimum occupancy.
         * Update sibling pointers as needed, and make the right page available for reuse.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param leftPage - the left leaf page
         * @param rightPage - the right leaf page
         * @param parent - the parent of the two pages
         * @param parentEntry - the entry in the parent corresponding to the leftPage and rightPage
         * @see deleteParentEntry
         */
        void mergeLeafPages(TransactionId tid, PagesMap &dirtypages,
                            BTreeLeafPage *leftPage, BTreeLeafPage *rightPage, BTreeInternalPage *parent,
                            BTreeEntry *parentEntry);

        /**
         * Merge two internal pages by moving all entries from the right page to the left page 
         * and "pulling down" the corresponding key from the parent entry. 
         * Delete the corresponding key and right child pointer from the parent, and
         * handle the case when the parent gets below minimum occupancy.
         * Update parent pointers as needed, and make the right page available for reuse.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param leftPage - the left internal page
         * @param rightPage - the right internal page
         * @param parent - the parent of the two pages
         * @param parentEntry - the entry in the parent corresponding to the leftPage and rightPage
         * @see deleteParentEntry
         * @see updateParentPointers
         */
        void mergeInternalPages(TransactionId tid, PagesMap &dirtypages,
                                BTreeInternalPage *leftPage, BTreeInternalPage *rightPage, BTreeInternalPage *parent,
                                BTreeEntry *parentEntry);

        /**
         * Steal entries from the left sibling and copy them to the given page so that both pages are at least
         * half full. Keys can be thought of as rotating through the parent entry, so the original key in the 
         * parent is "pulled down" to the right-hand page, and the last key in the left-hand page is "pushed up"
         * to the parent.  Update parent pointers as needed.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param page - the internal page which is less than half full
         * @param leftSibling - the left sibling which has entries to spare
         * @param parent - the parent of the two internal pages
         * @param parentEntry - the entry in the parent pointing to the two internal pages
         * @see updateParentPointers
         */
        void stealFromLeftInternalPage(TransactionId tid, PagesMap &dirtypages,
                                       BTreeInternalPage *page, BTreeInternalPage *leftSibling,
                                       BTreeInternalPage *parent, BTreeEntry *parentEntry);

        /**
         * Get the page number of the first empty page in this BTreeFile.
         * Creates a new page if none of the existing pages are empty.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @return the page number of the first empty page
         */
        int getEmptyPageNo(TransactionId tid, PagesMap &dirtypages);

        /**
         * Mark a page in this BTreeFile as empty. Find the corresponding header page 
         * (create it if needed), and mark the corresponding slot in the header page as empty.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param emptyPageNo - the page number of the empty page
         * @see getEmptyPage
         */
        void setEmptyPage(TransactionId tid, PagesMap &dirtypages, int emptyPageNo);

    public:
        /**
         * Constructs a B+ tree file backed by the specified file.
         * 
         * @param fname - the file that stores the on-disk backing store for this B+ tree file.
         * @param key - the field which index is keyed on
         * @param td - the tuple descriptor of tuples in the file
         */
        BTreeFile(const char *fname, int key, const TupleDesc &td);

        /**
         * Returns an ID uniquely identifying this BTreeFile. Implementation note:
         * you will need to generate this tableid somewhere and ensure that each
         * BTreeFile has a "unique id," and that you always return the same value for
         * a particular BTreeFile. We suggest hashing the absolute file name of the
         * file underlying the BTreeFile, i.e. f.getAbsoluteFile().hashCode().
         * 
         * @return an ID uniquely identifying this BTreeFile.
         */
        int getId() const override;

        /**
         * Returns the TupleDesc of the table stored in this DbFile.
         * 
         * @return TupleDesc of this DbFile.
         */
        const TupleDesc &getTupleDesc() const override;

        /**
         * Read a page from the file on disk. This should not be called directly
         * but should be called from the BufferPool via getPage()
         * 
         * @param pid - the id of the page to read from disk
         * @return the page constructed from the contents on disk
         */
        Page *readPage(const PageId &pid) override;

        /**
         * Write a page to disk.  This should not be called directly but should 
         * be called from the BufferPool when pages are flushed to disk
         * 
         * @param page - the page to write to disk
         */
        void writePage(Page *page) override;

        /**
         * Returns the number of pages in this BTreeFile.
         */
        int getNumPages() const;

        /**
         * Returns the index of the field that this B+ tree is keyed on
         */
        int getKeyField() const;

        /**
         * Convenience method to find a leaf page without dirtypages.
         * Used by the BTreeFile iterator.
         * @see findLeafPage
         *
         * @param tid - the transaction id
         * @param pid - the current page being searched
         * @param perm - the permissions with which to lock the leaf page
         * @param f - the field to search for
         * @return the left-most leaf page possibly containing the key field f
         * 
         */
        BTreeLeafPage *findLeafPage(TransactionId tid, BTreePageId *pid, Permissions perm, const Field *f);

        /**
         * Method to encapsulate the process of locking/fetching a page.  First the method checks the local 
         * cache ("dirtypages"), and if it can't find the requested page there, it fetches it from the buffer pool.  
         * It also adds pages to the dirtypages cache if they are fetched with read-write permission, since 
         * presumably they will soon be dirtied by this transaction.
         * 
         * This method is needed to ensure that page updates are not lost if the same pages are
         * accessed multiple times.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages
         * @param pid - the id of the requested page
         * @param perm - the requested permissions on the page
         * @return the requested page
         */
        Page *getPage(TransactionId tid, PagesMap &dirtypages, const BTreePageId *pid, Permissions perm);

        /**
         * Insert a tuple into this BTreeFile, keeping the tuples in sorted order. 
         * May cause pages to split if the page where tuple t belongs is full.
         * 
         * @param tid - the transaction id
         * @param t - the tuple to insert
         * @return a list of all pages that were dirtied by this operation. Could include
         * many pages since parent pointers will need to be updated when an internal node splits.
         * @see splitLeafPage
         */
        std::vector<Page *> insertTuple(TransactionId tid, Tuple &t) override;

        /**
         * Delete a tuple from this BTreeFile. 
         * May cause pages to merge or redistribute entries/tuples if the pages 
         * become less than half full.
         * 
         * @param tid - the transaction id
         * @param t - the tuple to delete
         * @return a list of all pages that were dirtied by this operation. Could include
         * many pages since parent pointers will need to be updated when an internal node merges.
         * @see handleMinOccupancyPage
         */
        std::vector<Page *> deleteTuple(TransactionId tid, Tuple &t) override;

        /**
         * Get a read lock on the root pointer page. Create the root pointer page and root page
         * if necessary.
         * 
         * @param tid - the transaction id
         * @param dirtypages - the list of dirty pages which should be updated with all new dirty pages 
         * @return the root pointer page
         */
        BTreeRootPtrPage *getRootPtrPage(TransactionId tid, PagesMap &dirtypages);

        /**
         * Get the specified tuples from the file based on its IndexPredicate value on
         * behalf of the specified transaction. This method will acquire a read lock on
         * the affected pages of the file, and may block until the lock can be
         * acquired.
         *
         * @param tid - the transaction id
         * @param ipred - the index predicate value to filter on. If null, return all tuples
         * @return an iterator for the filtered tuples
         */
        BTreeIterable iterable(TransactionId tid, IndexPredicate *ipred = nullptr);
    };
}

#endif
