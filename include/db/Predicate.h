#ifndef DB_PREDICATE_H
#define DB_PREDICATE_H

#include <string>
#include <cassert>

namespace db {
    class Tuple;
    class Field;
    /**
     * Predicate compares tuples to a specified Field value.
     */
    class Predicate {
    public:
        enum class Op {
            EQUALS, NOT_EQUALS, GREATER_THAN, LESS_THAN, LESS_THAN_OR_EQ, GREATER_THAN_OR_EQ, LIKE
        };
    private:
        int field;
        Predicate::Op op;
        const Field *operand;

    public:
        /**
         * Constructor.
         *
         * @param field
         *            field number of passed in tuples to compare against.
         * @param op
         *            operation to use for comparison
         * @param operand
         *            field value to compare passed in tuples to
         */
        Predicate(int field, Op op, const Field *operand);

        /**
         * @return the field number
         */
        int getField() const;

        /**
         * @return the operator
         */
        Op getOp() const;

        /**
         * @return the operand
         */
        const Field *getOperand() const;

        /**
         * Compares the field number of t specified in the constructor to the
         * operand field specified in the constructor using the operator specific in
         * the constructor. The comparison can be made through Field's compare
         * method.
         *
         * @param t
         *            The tuple to compare against
         * @return true if the comparison is true, false otherwise.
         */
        bool filter(const Tuple &t) const;

        /**
         * Returns something useful, like "f = field_id op = op_string operand =
         * operand_string
         */
        std::string to_string() const;
    };

    std::string to_string(Predicate::Op op);
}
#endif
