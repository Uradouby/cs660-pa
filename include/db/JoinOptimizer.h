#ifndef DB_JOINOPTIMIZER_H
#define DB_JOINOPTIMIZER_H


#include <db/CostCard.h>
#include <db/PlanCache.h>
#include <db/TableStats.h>
#include <db/Predicate.h>
#include <db/LogicalJoinNode.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace db {

/**
 * The JoinOptimizer class is responsible for ordering a series of joins
 * optimally, and for selecting the best instantiation of a join for a given
 * logical plan.
 */
    class JoinOptimizer {

    /**
     * This is a helper method that computes the cost and cardinality of joining
     * joinToRemove to joinSet (joinSet should contain joinToRemove), given that
     * all of the subsets of size joinSet.size() - 1 have already been computed
     * and stored in PlanCache pc.
     *
     * @param stats
     *            table stats for all of the tables, referenced by table names
     *            rather than alias (see {@link #orderJoins})
     * @param filterSelectivities
     *            the selectivities of the filters over each of the tables
     *            (where tables are indentified by their alias or name if no
     *            alias is given)
     * @param joinToRemove
     *            the join to remove from joinSet
     * @param joinSet
     *            the set of joins being considered
     * @param bestCostSoFar
     *            the best way to join joinSet so far (minimum of previous
     *            invocations of computeCostAndCardOfSubplan for this joinSet,
     *            from returned CostCard)
     * @param pc
     *            the PlanCache for this join; should have subplans for all
     *            plans of size joinSet.size()-1
     * @return A {@link CostCard} objects describing the cost, cardinality, optimal subplan
     */
    virtual CostCard *computeCostAndCardOfSubplan(
            std::unordered_map<std::string, TableStats> stats,
            std::unordered_map<std::string, double> filterSelectivities,
            LogicalJoinNode joinToRemove, std::unordered_set<LogicalJoinNode> joinSet,
            double bestCostSoFar, PlanCache pc) = 0;

//        LogicalPlan p;
        std::vector<LogicalJoinNode> joins;
    public:
        /**
         * Constructor
         *
         * @param p
         *            the logical plan being optimized
         * @param joins
         *            the list of joins being performed
         */
//        JoinOptimizer(LogicalPlan p, std::vector<LogicalJoinNode> &joins) {
        JoinOptimizer(std::vector<LogicalJoinNode> &joins) {
//            this->p = p;
            this->joins = joins;
        }

        /**
          * Estimate the cost of a join.
          *
          * The cost of the join should be a function of the amount of data that must
          * be read over the course of the query, as well as the number of CPU operations
          * performed by your join.
          * Assume that the cost of a single predicate application is roughly 1.
          *
          * @param card1
          *            Estimated cardinality of the left-hand side of the query
          * @param card2
          *            Estimated cardinality of the right-hand side of the query
          * @param cost1
          *            Estimated cost of one full scan of the table on the left-hand side of the query
          * @param cost2
          *            Estimated cost of one full scan of the table on the right-hand side of the query
          * @return An estimate of the cost of this query, in terms of cost1 and cost2
          */
        static double estimateJoinCost(int card1, int card2, double cost1, double cost2);

        /**
         * Estimate the join cardinality of two tables.
         */
        static int estimateTableJoinCardinality(Predicate::Op joinOp,
                                                const std::string &table1Alias, const std::string &table2Alias,
                                                const std::string &field1PureName, const std::string &field2PureName,
                                                int card1, int card2, bool t1pkey, bool t2pkey,
                                                const std::unordered_map<std::string, TableStats> &stats,
                                                const std::unordered_map<std::string, int> &tableAliasToId);


        /**
         * Compute a logical, reasonably efficient join on the specified tables. See
         * PS4 for hints on how this should be implemented.
         *
         * @param stats
         *            Statistics for each table involved in the join, referenced by
         *            base table names, not alias
         * @param filterSelectivities
         *            Selectivities of the filter predicates on each table in the
         *            join, referenced by table alias (if no alias, the base table
         *            name)
         * @return A std::vector<LogicalJoinNode> that stores joins in the left-deep
         *         order in which they should be executed.
         * @throws ParsingException
         *             when stats or filter selectivities is missing a table in the
         *             join, or or when another internal error occurs
         */
        std::vector<LogicalJoinNode> orderJoins(std::unordered_map<std::string, TableStats> stats,
                                                std::unordered_map<std::string, double> filterSelectivities);

        /**
         * Helper method to enumerate all of the subsets of a given size of a
         * specified vector.
         *
         * @param v
         *            The vector whose subsets are desired
         * @param size
         *            The size of the subsets of interest
         * @return a set of all subsets of the specified size
         */
        template<typename T>
        static std::vector<std::unordered_set<T>> enumerateSubsets(std::vector<T> v, int size) {
            std::vector<std::unordered_set<T>> ret;
            if (size == 0) {
                std::unordered_set<T> empty;
                ret.push_back(empty);
                return ret;
            }
            for (int i = 0; i < v.size(); i++) {
                std::vector<T> copy = v;
                copy.erase(copy.begin() + i);
                for (auto s : enumerateSubsets(copy, size - 1)) {
                    s.insert(v[i]);
                    ret.push_back(s);
                }
            }
            return ret;
        }
    };

}

#endif
