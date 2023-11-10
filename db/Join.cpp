#include <db/Join.h>

using namespace db;

Join::Join(JoinPredicate *p, DbIterator *child1, DbIterator *child2) {
    // TODO pa3.1: some code goes here
}

JoinPredicate *Join::getJoinPredicate() {
    // TODO pa3.1: some code goes here
}

std::string Join::getJoinField1Name() {
    // TODO pa3.1: some code goes here
}

std::string Join::getJoinField2Name() {
    // TODO pa3.1: some code goes here
}

const TupleDesc &Join::getTupleDesc() const {
    // TODO pa3.1: some code goes here
}

void Join::open() {
    // TODO pa3.1: some code goes here
}

void Join::close() {
    // TODO pa3.1: some code goes here
}

void Join::rewind() {
    // TODO pa3.1: some code goes here
}

std::vector<DbIterator *> Join::getChildren() {
    // TODO pa3.1: some code goes here
}

void Join::setChildren(std::vector<DbIterator *> children) {
    // TODO pa3.1: some code goes here
}

std::optional<Tuple> Join::fetchNext() {
    // TODO pa3.1: some code goes here
}
