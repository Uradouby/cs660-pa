#include <db/Aggregate.h>
#include <db/IntegerAggregator.h>
#include <db/StringAggregator.h>

using namespace db;

std::optional<Tuple> Aggregate::fetchNext() {
    // TODO pa3.2: some code goes here
}

Aggregate::Aggregate(DbIterator *child, int afield, int gfield, Aggregator::Op aop) {
    // TODO pa3.2: some code goes here
}

int Aggregate::groupField() {
    // TODO pa3.2: some code goes here
}

std::string Aggregate::groupFieldName() {
    // TODO pa3.2: some code goes here
}

int Aggregate::aggregateField() {
    // TODO pa3.2: some code goes here
}

std::string Aggregate::aggregateFieldName() {
    // TODO pa3.2: some code goes here
}

Aggregator::Op Aggregate::aggregateOp() {
    // TODO pa3.2: some code goes here
}

void Aggregate::open() {
    // TODO pa3.2: some code goes here
}

void Aggregate::rewind() {
    // TODO pa3.2: some code goes here
}

const TupleDesc &Aggregate::getTupleDesc() const {
    // TODO pa3.2: some code goes here
}

void Aggregate::close() {
    // TODO pa3.2: some code goes here
}

std::vector<DbIterator *> Aggregate::getChildren() {
    // TODO pa3.2: some code goes here
}

void Aggregate::setChildren(std::vector<DbIterator *> children) {
    // TODO pa3.2: some code goes here
}
