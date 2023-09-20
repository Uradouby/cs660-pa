#ifndef DB_PAGE_H
#define DB_PAGE_H

#include <db/PageId.h>
#include <db/TransactionId.h>

namespace db {
    class Page {
    public:
        virtual PageId &getId() = 0;

        virtual void *getPageData() = 0;

        virtual ~Page() = default;
    };
}

#endif
