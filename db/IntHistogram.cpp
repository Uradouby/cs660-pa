#include <db/IntHistogram.h>

using namespace db;

IntHistogram::IntHistogram(int buckets, int min, int max) {
    // TODO pa4.1: some code goes here
}

void IntHistogram::addValue(int v) {
    // TODO pa4.1: some code goes here
}

double IntHistogram::estimateSelectivity(Predicate::Op op, int v) const {
    // TODO pa4.1: some code goes here
}

double IntHistogram::avgSelectivity() const {
    // TODO pa4.1: some code goes here
}

std::string IntHistogram::to_string() const {
    // TODO pa4.1: some code goes here
}
