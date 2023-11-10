#include <db/Operator.h>

using namespace db;

void Operator::setEstimatedCardinality(int card) {
    estimatedCardinality = card;
}

bool Operator::hasNext() {
    if (!isOpen)
        throw std::runtime_error("Operator not open");

    if (!tup.has_value())
        tup = fetchNext();
    return tup.has_value();
}

Tuple Operator::next() {
    if (!tup.has_value()) {
        tup = fetchNext();
        if (!tup.has_value())
            throw std::runtime_error("No more tuples");
    }

    Tuple result = tup.value();
    tup = std::nullopt;
    return result;
}

void Operator::open() {
    isOpen = true;
}

void Operator::close() {
    // Ensures that a future call to next() will fail
    tup = std::nullopt;
    isOpen = false;
}

void Operator::rewind() {
    close();
    open();
}

int Operator::getEstimatedCardinality() const {
    return estimatedCardinality;
}
