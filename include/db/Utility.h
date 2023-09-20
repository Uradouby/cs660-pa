#ifndef DB_UTILITY_H
#define DB_UTILITY_H

#include <db/TupleDesc.h>
#include <string>

namespace db::Utility {
    TupleDesc getTupleDesc(int numFields);

    TupleDesc getTupleDesc(int numFields, const std::string &prefix);

    int randomInt();

    std::string generateUUID();
}

#endif
