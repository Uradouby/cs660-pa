#ifndef DB_COSTCARD_H
#define DB_COSTCARD_H

#include <vector>
#include <db/LogicalJoinNode.h>

namespace db {

    /**
     * Class returned by {@link JoinOptimizer#computeCostAndCardOfSubplan}
     * specifying the cost and cardinality of the optimal plan represented by plan.
     */
    class CostCard {
    public:
        /** The cost of the optimal subplan */
        double cost;
        /** The cardinality of the optimal subplan */
        int card;
        /** The optimal subplan */
        std::vector<LogicalJoinNode> plan;
    };

}
#endif
