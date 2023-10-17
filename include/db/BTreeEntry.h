#ifndef DB_BTREE_ENTRY_H
#define DB_BTREE_ENTRY_H

#include <db/BTreePageId.h>
#include <db/RecordId.h>
#include <db/Field.h>

#include <string>

namespace db {
    /**
     * Each instance of BTreeEntry stores one key and two child page ids. It is used
     * by BTreeInternalPage as an abstraction to iterate through the entries stored inside.
     * All of the entries or tuples in the left child page should be less than or equal to
     * the key, and all of the entries or tuples in the right child page should be greater
     * than or equal to the key.
     *
     * Note that updating a BTreeEntry does not actually change the data stored on the page
     * identified by its recordId. After updating a BTreeEntry object, you must call
     * BTreeInternalPage::updateEntry in order for the changes to take effect.
     *
     * @see BTreeInternalPage
     * @see BTreeInternalPage::updateEntry
     *
     */
    class BTreeEntry {

        /**
         * The key of this entry
         */
        Field *key;

        /**
         * The left child page id
         */
        BTreePageId *leftChild;

        /**
         * The right child page id
         */
        BTreePageId *rightChild;

        /**
         * The record id of this entry
         */
        RecordId *rid; // null if not stored on any page

    public:
        /**
         * Constructor to create a new BTreeEntry
         * @param field - the key
         * @param lcid - page id of the left child
         * @param rcid - page id of the right child
         */
        BTreeEntry(Field *field, BTreePageId *lcid, BTreePageId *rcid);

        /**
         * @return the key
         */
        Field *getKey() const;

        /**
         * @return the left child page id
         */
        BTreePageId *getLeftChild() const;

        /**
         * @return the right child page id
         */
        BTreePageId *getRightChild() const;

        /**
         * @return the record id of this entry, representing the location of this entry
         * in a BTreeFile. May be null if this entry is not stored on any page in the file
         */
        RecordId *getRecordId() const;

        /**
         * Set the key for this entry. Note that updating a BTreeEntry does not
         * actually change the data stored on the page identified by its recordId.  After
         * calling this method, you must call BTreeInternalPage.updateEntry() in order for
         * it to take effect.
         * @param field - the new key
         * @see BTreeInternalPage#updateEntry(BTreeEntry)
         */
        void setKey(Field *field);

        /**
         * Set the left child id for this entry.  Note that updating a BTreeEntry does not
         * actually change the data stored on the page identified by its recordId.  After
         * calling this method, you must call BTreeInternalPage.updateEntry() in order for
         * it to take effect.
         * @param id - the new left child
         * @see BTreeInternalPage#updateEntry(BTreeEntry)
         */
        void setLeftChild(BTreePageId *id);

        /**
         * Set the right child id for this entry.  Note that updating a BTreeEntry does not
         * actually change the data stored on the page identified by its recordId.  After
         * calling this method, you must call BTreeInternalPage.updateEntry() in order for
         * it to take effect.
         * @param id - the new right child
         * @see BTreeInternalPage#updateEntry(BTreeEntry)
         */
        void setRightChild(BTreePageId *id);

        /**
         * set the record id for this entry
         * @param id - the new record id
         */
        void setRecordId(RecordId *id);

        /**
         * Prints a representation of this BTreeEntry
         */
        std::string to_string() const;

    };
}

#endif
