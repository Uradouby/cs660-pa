#include <db/HashEquiJoin.h>

using namespace db;

HashEquiJoin::HashEquiJoin(JoinPredicate p, DbIterator *child1, DbIterator *child2) {
    // TODO pa3.1: some code goes here
}

JoinPredicate *HashEquiJoin::getJoinPredicate() {
    // TODO pa3.1: some code goes here
}

const TupleDesc &HashEquiJoin::getTupleDesc() const {
    // TODO pa3.1: some code goes here
}

std::string HashEquiJoin::getJoinField1Name() {
    // TODO pa3.1: some code goes here
}

std::string HashEquiJoin::getJoinField2Name() {
    // TODO pa3.1: some code goes here
}

void HashEquiJoin::open() {
    // TODO pa3.1: some code goes here
}

void HashEquiJoin::close() {
    // TODO pa3.1: some code goes here
}

void HashEquiJoin::rewind() {
    // TODO pa3.1: some code goes here
}

std::vector<DbIterator *> HashEquiJoin::getChildren() {
    // TODO pa3.1: some code goes here
}

void HashEquiJoin::setChildren(std::vector<DbIterator *> children) {
    // TODO pa3.1: some code goes here
}

std::optional<Tuple> HashEquiJoin::fetchNext() {
    // TODO pa3.1: some code goes here
}
