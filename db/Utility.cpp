#include <random>
#include <db/Utility.h>

using namespace db;

TupleDesc Utility::getTupleDesc(int numFields) {
    std::vector<Types::Type> types(numFields, Types::INT_TYPE);
    return TupleDesc(types);
}

TupleDesc Utility::getTupleDesc(int numFields, const std::string &prefix) {
    std::vector<Types::Type> types(numFields, Types::INT_TYPE);
    std::vector<std::string> names(numFields);
    for (int i = 0; i < numFields; ++i) {
        names[i] = prefix + std::to_string(i);
    }
    return TupleDesc(types, names);
}

static int uuid;

std::string Utility::generateUUID() {
    return std::to_string(uuid++);
}

static std::mt19937 gen;
static std::uniform_int_distribution<int> dist;

int Utility::randomInt() {
    return dist(gen);
}
