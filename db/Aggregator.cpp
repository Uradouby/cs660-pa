#include <db/Aggregator.h>

using namespace db;

std::string to_string(Aggregator::Op op) {
    switch (op) {
        case Aggregator::Op::MIN:
            return "min";
        case Aggregator::Op::MAX:
            return "max";
        case Aggregator::Op::SUM:
            return "sum";
        case Aggregator::Op::AVG:
            return "avg";
        case Aggregator::Op::COUNT:
            return "count";
    }
    throw std::runtime_error("impossible to reach here");
}
