#ifndef DB_PAGESMAP_H
#define DB_PAGESMAP_H

#include <db/Page.h>
#include <db/PageId.h>
#include <unordered_map>
#include <cstring>

namespace db {
    struct hasher {
        size_t operator()(const PageId *pid) const {
            return std::hash<PageId>{}(*pid);
        }
    };

    struct equals {
        bool operator()(const PageId *pid1, const PageId *pid2) const {
            if (pid1 == nullptr && pid2 == nullptr)
                return true;
            return *pid1 == *pid2;
        }
    };

    using PagesMap = std::unordered_map<const PageId *, Page *, hasher, equals>;
}

#endif
