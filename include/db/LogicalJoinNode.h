#ifndef DB_LOGICALJOINNODE_H
#define DB_LOGICALJOINNODE_H

#include <string>
#include <db/Predicate.h>

namespace db {

/**
 * A LogicalJoinNode represents the state needed of a join of two tables in a LogicalQueryPlan
 */
    class LogicalJoinNode {
    public:
        /** The first table to join (may be null). It's the alias of the table (if no alias, the true table name) */
        std::string t1Alias;

        /** The second table to join (may be null).  It's the alias of the table, (if no alias, the true table name).*/
        std::string t2Alias;

        /** The name of the field in t1 to join with. It's the pure name of a field, rather that alias.field. */
        std::string f1PureName;

        std::string f1QuantifiedName;

        /** The name of the field in t2 to join with. It's the pure name of a field.*/
        std::string f2PureName;

        std::string f2QuantifiedName;

        /** The join predicate */
        Predicate::Op p;

        LogicalJoinNode() = default;

        LogicalJoinNode(const std::string &table1, const std::string &table2,
                        const std::string &joinField1, const std::string &joinField2,
                        Predicate::Op pred) {
            t1Alias = table1;
            t2Alias = table2;
            // get joinField1 after last period
            f1PureName = joinField1.substr(joinField1.find_last_of('.') + 1);
            f2PureName = joinField2.substr(joinField2.find_last_of('.') + 1);
            p = pred;
            f1QuantifiedName = t1Alias + "." + f1PureName;
            f2QuantifiedName = t2Alias + "." + f2PureName;
        }

        /**
         * Return a new LogicalJoinNode with the inner and outer (t1.f1 and t2.f2) tables swapped.
         */
        LogicalJoinNode swapInnerOuter() const {
            Predicate::Op newp;
            if (p == Predicate::Op::GREATER_THAN)
                newp = Predicate::Op::LESS_THAN;
            else if (p == Predicate::Op::GREATER_THAN_OR_EQ)
                newp = Predicate::Op::LESS_THAN_OR_EQ;
            else if (p == Predicate::Op::LESS_THAN)
                newp = Predicate::Op::GREATER_THAN;
            else if (p == Predicate::Op::LESS_THAN_OR_EQ)
                newp = Predicate::Op::GREATER_THAN_OR_EQ;
            else
                newp = p;

            return {t2Alias, t1Alias, f2PureName, f1PureName, newp};
        }

        std::string to_string() const {
            return t1Alias + ":" + t2Alias;//+ ";" + f1 + " " + p + " " + f2;
        }

        bool operator==(const LogicalJoinNode &rhs) const {
            return (t1Alias == rhs.t1Alias && t2Alias == rhs.t2Alias) ||
                   (t1Alias == rhs.t2Alias && t2Alias == rhs.t1Alias);
        }

        bool operator!=(const LogicalJoinNode &rhs) const {
            return !(rhs == *this);
        }
    };

}

template<>
struct std::hash<db::LogicalJoinNode> {
    std::size_t operator()(const db::LogicalJoinNode &node) const {
        std::hash<std::string> hash;
        return hash(node.t1Alias) ^ hash(node.t2Alias) ^ hash(node.f1PureName) ^ hash(node.f2PureName);
    }
};

#endif
