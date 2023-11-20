#ifndef DB_PLANCACHE_H
#define DB_PLANCACHE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstdlib>
#include <db/LogicalJoinNode.h>
#include <db/CostCard.h>

namespace db {

    struct Hash {
        template<typename T>
        size_t operator()(const std::unordered_set<T> &s) const {
            size_t hash = 0;
            for (const auto &val: s) {
                hash ^= std::hash<T>{}(val);
            }
            return hash;
        }
    };

    /**
     * A PlanCache is a helper class that can be used to store the best way to order a given set of joins
     */
    class PlanCache {
        std::unordered_map<std::unordered_set<LogicalJoinNode>, CostCard *, Hash> best;
        std::unordered_map<std::unordered_set<LogicalJoinNode>, double, Hash> bestCosts;
        std::unordered_map<std::unordered_set<LogicalJoinNode>, int, Hash> bestCardinalities;
    public:
        /**
         * Add a new cost, cardinality and ordering for a particular join set.  Does not verify that the
         * new cost is less than any previously added cost -- simply adds or replaces an existing plan for the
         * specified join set
         * @param s the set of joins for which a new ordering (plan) is being added
         * @param cost the estimated cost of the specified plan
         * @param card the estimated cardinality of the specified plan
         * @param order the ordering of the joins in the plan
         */
        void
        addPlan(std::unordered_set<LogicalJoinNode> &s, CostCard *card) {
            best[s] = card;
        }

        CostCard *get(std::unordered_set<LogicalJoinNode> &s) {
            return best[s];
        }
    };
}

#endif
