#include <db/Filter.h>

using namespace db;

Filter::Filter(Predicate p, DbIterator *child) {
    // TODO pa3.1: some code goes here
}

Predicate *Filter::getPredicate() {
    // TODO pa3.1: some code goes here
}

const TupleDesc &Filter::getTupleDesc() const {
    // TODO pa3.1: some code goes here
}

void Filter::open() {
    // TODO pa3.1: some code goes here
}

void Filter::close() {
    // TODO pa3.1: some code goes here
}

void Filter::rewind() {
    // TODO pa3.1: some code goes here
}

std::vector<DbIterator *> Filter::getChildren() {
    // TODO pa3.1: some code goes here
}

void Filter::setChildren(std::vector<DbIterator *> children) {
    // TODO pa3.1: some code goes here
}

std::optional<Tuple> Filter::fetchNext() {
    // TODO pa3.1: some code goes here
}
