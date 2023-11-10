#include <db/JoinPredicate.h>
#include <db/Tuple.h>

using namespace db;

JoinPredicate::JoinPredicate(int field1, Predicate::Op op, int field2) {
    // TODO pa3.1: some code goes here
}

bool JoinPredicate::filter(Tuple *t1, Tuple *t2) {
    // TODO pa3.1: some code goes here
}

int JoinPredicate::getField1() const {
    // TODO pa3.1: some code goes here
}

int JoinPredicate::getField2() const {
    // TODO pa3.1: some code goes here
}

Predicate::Op JoinPredicate::getOperator() const {
    // TODO pa3.1: some code goes here
}
