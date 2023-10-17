#ifndef DB_PREDICATE_H
#define DB_PREDICATE_H

#include <string>
#include <db/Tuple.h>
#include <db/Field.h>
#include <cassert>

namespace db {
    /**
     * Predicate compares tuples to a specified Field value.
     */
    class Predicate {
        // TODO pa2.2: add private members

        /** Constants used for return codes in Field.compare */
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
        std::string toString() const {
            return "f = " + std::to_string(getField()) + " op = " + to_string(getOp()) +
                   " operand = " + getOperand()->to_string();
        }
    };
}
#endif
