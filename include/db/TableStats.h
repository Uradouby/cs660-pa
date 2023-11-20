#ifndef DB_TABLESTATS_H
#define DB_TABLESTATS_H

#include <unordered_map>
#include <string>
#include <iostream>
#include <db/Database.h>
#include <db/IntHistogram.h>
#include <db/IntField.h>
#include <db/StringField.h>
#include <climits>
#include "SeqScan.h"

namespace db {
    /**
     * TableStats represents statistics (e.g., histograms) about base tables in a
     * query.
     *
     * This class is not needed in implementing lab1, lab2 and lab3.
     */
    class TableStats {
        static std::unordered_map<std::string, TableStats *> statsMap;

        int baseTups;
        int basePages;
        int costPerPageIO;
        std::vector<Histogram *> histograms;
        std::vector<int> maxs;
        std::vector<int> mins;
        TupleDesc td;

        /**
         * Number of bins for the histogram. Feel free to increase this value over
         * 100, though our tests assume that you have at least 100 bins in your
         * histograms.
         */
        static constexpr int NUM_HIST_BINS = 100;
        static constexpr int IOCOSTPERPAGE = 1000;

    public:

        static TableStats *getTableStats(std::string tablename) {
            return statsMap.at(tablename);
        }

        static void setTableStats(std::string tablename, TableStats *stats) {
            statsMap[tablename] = stats;
        }

        static void setStatsMap(std::unordered_map<std::string, TableStats *> s) {
            statsMap = s;
        }

        static std::unordered_map<std::string, TableStats *> getStatsMap() {
            return statsMap;
        }

        static void computeStatistics() {
            std::cout << "Computing table stats." << std::endl;
            for (const auto &item: Database::getCatalog()) {
                auto id = item.first;
                auto name = item.second->name;
                auto s = new TableStats(id, IOCOSTPERPAGE);
                setTableStats(name, s);
            }
            std::cout << "Done.\n" << std::endl;
        }

        /**
         * Create a new TableStats object, that keeps track of statistics on each
         * column of a table
         *
         * @param tableid
         *            The table over which to compute statistics
         * @param ioCostPerPage
         *            The cost per page of IO. This doesn't differentiate between
         *            sequential-scan IO and disk seeks.
         */
        TableStats(int tableid, int ioCostPerPage) {
            // For this function, you'll have to get the
            // DbFile for the table in question,
            // then scan through its tuples and calculate
            // the values that you need.
            // You should try to do this reasonably efficiently, but you don't
            // necessarily have to (for example) do everything
            // in a single scan of the table.
            auto f = Database::getCatalog().getDatabaseFile(tableid);
            td = f->getTupleDesc();

            HeapFile *hf = dynamic_cast<HeapFile *>(f);
            if (hf == nullptr) {
                basePages = 0;
                baseTups = 0;
                costPerPageIO = ioCostPerPage;
                histograms = {};
                mins = maxs = {};
                return;
            }

            costPerPageIO = ioCostPerPage;
            histograms.resize(td.numFields());
            maxs.resize(td.numFields());
            mins.resize(td.numFields());
            for (int i = 0; i < td.numFields(); i++) {
                maxs[i] = INT_MIN;
                mins[i] = INT_MAX;
            }
            // scan the data once to determine the min and max values
            SeqScan s1(tableid, "t");
            s1.open();
            while (s1.hasNext()) {
                Tuple tup = s1.next();
                for (int i = 0; i < td.numFields(); i++) {
                    if (td.getFieldType(i) == Types::Type::INT_TYPE) {
                        const IntField &ffield = dynamic_cast<const IntField &>(tup.getField(i));
                        int v = ffield.getValue();
                        if (v > maxs[i])
                            maxs[i] = v;
                        if (v < mins[i])
                            mins[i] = v;
                    }
                }
            }

            for (int i = 0; i < td.numFields(); i++) {
                if (td.getFieldType(i) == Types::Type::INT_TYPE) {
                    histograms[i] = new IntHistogram(NUM_HIST_BINS, mins[i], maxs[i]);
                } else {
                    histograms[i] = nullptr;
                }
            }

            basePages = f->getNumPages();
            int count = 0;
            SeqScan s2(tableid, "t");
            s2.open();
            while (s2.hasNext()) { // scan again to populate histograms
                Tuple tup = s2.next();
                count++;
                for (int i = 0; i < td.numFields(); i++) {
                    if (td.getFieldType(i) == Types::Type::INT_TYPE) {
                        int v = ((IntField &) tup.getField(i)).getValue();
                        ((IntHistogram *) histograms[i])->addValue(v);
                    }
                }
            }

            baseTups = count;
        }

        /**
         * Estimates the cost of sequentially scanning the file, given that the cost
         * to read a page is costPerPageIO. You can assume that there are no seeks
         * and that no pages are in the buffer pool.
         *
         * Also, assume that your hard drive can only read entire pages at once, so
         * if the last page of the table only has one tuple on it, it's just as
         * expensive to read as a full page. (Most real hard drives can't
         * efficiently address regions smaller than a page at a time.)
         *
         * @return The estimated cost of scanning the table.
         */
        double estimateScanCost() {
            return basePages * costPerPageIO;
        }

        /**
         * This method returns the number of tuples in the relation, given that a
         * predicate with selectivity selectivityFactor is applied.
         *
         * @param selectivityFactor
         *            The selectivity of any predicates over the table
         * @return The estimated cardinality of the scan with the specified
         *         selectivityFactor
         */
        int estimateTableCardinality(double selectivityFactor) {
            return (int) (baseTups * selectivityFactor);
        }

        /**
         * The average selectivity of the field under op.
         * @param field
         *        the index of the field
         * @param op
         *        the operator in the predicate
         * The semantic of the method is that, given the table, and then given a
         * tuple, of which we do not know the value of the field, return the
         * expected selectivity. You may estimate this value from the histograms.
         * */
        double avgSelectivity(int field, Predicate::Op op) {
            if (op == Predicate::Op::EQUALS) {
                if (td.getFieldType(field) == Types::Type::INT_TYPE) {
                    return ((IntHistogram *) histograms[field])->avgSelectivity();
                }
            }
            return 1.0; // make something up
        }

        /**
         * Estimate the selectivity of predicate <tt>field op constant</tt> on the
         * table.
         *
         * @param field
         *            The field over which the predicate ranges
         * @param op
         *            The logical operation in the predicate
         * @param constant
         *            The value against which the field is compared
         * @return The estimated selectivity (fraction of tuples that satisfy) the
         *         predicate
         */
        double estimateSelectivity(int field, Predicate::Op op, Field *constant) {
            if (td.getFieldType(field) == Types::Type::INT_TYPE) {
                IntHistogram *hist = (IntHistogram *) histograms[field];
                return hist->estimateSelectivity(op, ((IntField *) constant)->getValue());
            }
            return 1.0; // make something up.
        }

        /**
         * return the total number of tuples in this table
         * */
        int totalTuples() {
            return baseTups;
        }
    };
}

#endif

