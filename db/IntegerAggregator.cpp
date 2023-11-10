#include <db/IntegerAggregator.h>
#include <db/IntField.h>

using namespace db;

class IntegerAggregatorIterator : public DbIterator {
private:
    // TODO pa3.2: some code goes here
public:
    IntegerAggregatorIterator(int gbfield,
                              const TupleDesc &td,
                              const std::unordered_map<Field *, int> &count) {
        // TODO pa3.2: some code goes here
    }

    void open() override {
        // TODO pa3.2: some code goes here
    }

    bool hasNext() override {
        // TODO pa3.2: some code goes here
    }

    Tuple next() override {
        // TODO pa3.2: some code goes here
    }

    void rewind() override {
        // TODO pa3.2: some code goes here
    }

    const TupleDesc &getTupleDesc() const override {
        // TODO pa3.2: some code goes here
    }

    void close() override {
        // TODO pa3.2: some code goes here
    }
};

IntegerAggregator::IntegerAggregator(int gbfield, std::optional<Types::Type> gbfieldtype, int afield,
                                     Aggregator::Op what) {
    // TODO pa3.2: some code goes here
}

void IntegerAggregator::mergeTupleIntoGroup(Tuple *tup) {
    // TODO pa3.2: some code goes here
}

DbIterator *IntegerAggregator::iterator() const {
    // TODO pa3.2: some code goes here
}
