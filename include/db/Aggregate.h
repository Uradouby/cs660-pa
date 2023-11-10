#ifndef DB_AGGREGATE_H
#define DB_AGGREGATE_H

#include <string>
#include <vector>
#include <db/Tuple.h>
#include <db/Aggregator.h>
#include <db/Operator.h>
#include <db/DbIterator.h>

namespace db {
    /**
     * The Aggregation operator that computes an aggregate (e.g., sum, avg, max,
     * min). Note that we only support aggregates over a single column, grouped by a
     * single column.
     */
    class Aggregate : public Operator {
        // TODO pa3.2: add private members
    protected:
        /**
         * Returns the next tuple. If there is a group by field, then the first
         * field is the field by which we are grouping, and the second field is the
         * result of computing the aggregate, If there is no group by field, then
         * the result tuple should contain one field representing the result of the
         * aggregate. Should return null if there are no more tuples.
         */
        std::optional<Tuple> fetchNext() override;

    public:
        /**
         * Constructor.
         *
         * Implementation hint: depending on the type of afield, you will want to
         * construct an {@link IntAggregator} or {@link StringAggregator} to help
         * you with your implementation of readNext().
         *
         *
         * @param child
         *            The DbIterator * that is feeding us tuples.
         * @param afield
         *            The column over which we are computing an aggregate.
         * @param gfield
         *            The column over which we are grouping the result, or -1 if
         *            there is no grouping
         * @param aop
         *            The aggregation operator to use
         */
        Aggregate(DbIterator *child, int afield, int gfield, Aggregator::Op aop);

        /**
         * @return If this aggregate is accompanied by a groupby, return the groupby
         *         field index in the <b>INPUT</b> tuples. If not, return
         *         {@link Aggregator#NO_GROUPING}
         */
        int groupField();

        /**
         * @return If this aggregate is accompanied by a group by, return the name
         *         of the groupby field in the <b>OUTPUT</b> tuples If not, return
         *         null;
         */
        std::string groupFieldName();

        /**
         * @return the aggregate field
         */
        int aggregateField();

        /**
         * @return return the name of the aggregate field in the <b>OUTPUT</b>
         *         tuples
         */
        std::string aggregateFieldName();

        /**
         * @return return the aggregate operator
         */
        Aggregator::Op aggregateOp();

        static std::string nameOfAggregatorOp(Aggregator::Op aop) {
            return to_string(aop);
        }

        void open() override;

        void rewind() override;

        /**
         * Returns the TupleDesc of this Aggregate. If there is no group by field,
         * this will have one field - the aggregate column. If there is a group by
         * field, the first field will be the group by field, and the second will be
         * the aggregate value column.
         *
         * The name of an aggregate column should be informative. For example:
         * "aggName(aop) (child_td.getFieldName(afield))" where aop and afield are
         * given in the constructor, and child_td is the TupleDesc of the child
         * iterator.
         */
        const TupleDesc &getTupleDesc() const override;

        void close() override;

        std::vector<DbIterator *> getChildren() override;

        void setChildren(std::vector<DbIterator *> children) override;
    };
}

#endif
