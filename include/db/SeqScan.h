#ifndef DB_SEQSCAN_H
#define DB_SEQSCAN_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <db/TupleDesc.h>
#include <db/DbFile.h>
#include <db/HeapFile.h>

namespace db {
    /**
     * SeqScan is an implementation of a sequential scan access method that reads
     * each tuple of a table in no particular order (e.g., as they are laid out on
     * disk).
     */
    class SeqScan {
        using iterator = HeapFileIterator;
        int tableid;
        std::string alias;
        std::string tableName;
    public:

        /**
         * Creates a sequential scan over the specified table.
         *
         * @param tableid
         *            the table to scan.
         * @param tableAlias
         *            the alias of this table (needed by the parser); the returned
         *            tupleDesc should have fields with name tableAlias.fieldName
         *            (note: this class is not responsible for handling a case where
         *            tableAlias or fieldName are null. It shouldn't crash if they
         *            are, but the resulting name can be null.fieldName,
         *            tableAlias.null, or null.null).
         */
        SeqScan(int tableid, const std::string &tableAlias);

        /**
         * @return
         *       return the table name of the table the operator scans. This should
         *       be the actual name of the table in the catalog of the database
         * */
        std::string getTableName() const;

        /**
         * @return Return the alias of the table this operator scans.
         * */
        std::string getAlias() const;

        /**
         * Reset the tableid, and tableAlias of this operator.
         * @param tableid
         *            the table to scan.
         * @param tableAlias
         *            the alias of this table (needed by the parser); the returned
         *            tupleDesc should have fields with name tableAlias.fieldName
         *            (note: this class is not responsible for handling a case where
         *            tableAlias or fieldName are null. It shouldn't crash if they
         *            are, but the resulting name can be null.fieldName,
         *            tableAlias.null, or null.null).
         */
        void reset(int tableid, const std::string &tableAlias);

        SeqScan(int tableid);

        /**
         * Returns the TupleDesc with field names from the underlying HeapFile,
         * prefixed with the tableAlias string from the constructor. This prefix
         * becomes useful when joining tables containing a field(s) with the same
         * name.  The alias and name should be separated with a "." character
         * (e.g., "alias.fieldName").
         *
         * @return the TupleDesc with field names from the underlying HeapFile,
         *         prefixed with the tableAlias string from the constructor.
         */
        const TupleDesc &getTupleDesc() const;

        iterator begin() const;

        iterator end() const;
    };
}
#endif
