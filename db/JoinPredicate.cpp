#include <db/JoinPredicate.h>

using namespace db;

JoinPredicate::JoinPredicate(int field1, Predicate::Op op, int field2) {
    // TODO pa3.1: some code goes here
}

bool JoinPredicate::filter(Tuple *t1, Tuple *t2) {
    // TODO pa3.1: some code goes here
}

int JoinPredicate::getField1() {
    // TODO pa3.1: some code goes here
}

int JoinPredicate::getField2() {
    // TODO pa3.1: some code goes here
}

Predicate::Op JoinPredicate::getOperator() {
    // TODO pa3.1: some code goes here
}
