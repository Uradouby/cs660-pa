#include <db/StringAggregator.h>

using namespace db;

StringAggregator::StringAggregator(int gbfield, std::optional<Types::Type> gbfieldtype, int afield, Aggregator::Op what) {
    // TODO pa3.2: some code goes here
}

void StringAggregator::mergeTupleIntoGroup(Tuple *tup) {
    // TODO pa3.2: some code goes here
}

DbIterator *StringAggregator::iterator() const {
    // TODO pa3.2: some code goes here
}
