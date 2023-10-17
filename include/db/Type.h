#ifndef DB_TYPE_H
#define DB_TYPE_H

#include <cstdlib>
#include <cstdint>
#include <string>

namespace db {
    class Field;

    namespace Types {
        constexpr std::size_t STRING_LEN = 128;

        enum Type {
            INT_TYPE, STRING_TYPE
        };

        std::size_t getLen(Type type);

        Field *parse(uint8_t *data, Type type);

        std::string to_string(Type type);
    }
}

#endif
