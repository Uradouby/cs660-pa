#include <db/StringAggregator.h>

using namespace db;

StringAggregator::StringAggregator(int gbfield, std::optional<Types::Type> gbfieldtype, int afield, Aggregator::Op what) {
    // EXTRA pa3.2: some code goes here
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " not implemented");
}

void StringAggregator::mergeTupleIntoGroup(Tuple *tup) {
    // EXTRA pa3.2: some code goes here
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " not implemented");
}

DbIterator *StringAggregator::iterator() const {
    // EXTRA pa3.2: some code goes here
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " not implemented");
}
