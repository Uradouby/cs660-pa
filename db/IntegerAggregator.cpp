#include <db/IntegerAggregator.h>

using namespace db;

IntegerAggregator::IntegerAggregator(int gbfield, std::optional<Types::Type> gbfieldtype, int afield, Aggregator::Op what) {
    // TODO pa3.2: some code goes here
}

void IntegerAggregator::mergeTupleIntoGroup(Tuple *tup) {
    // TODO pa3.2: some code goes here
}

DbIterator *IntegerAggregator::iterator() const {
    // TODO pa3.2: some code goes here
}
