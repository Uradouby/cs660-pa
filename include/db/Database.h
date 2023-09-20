#ifndef DB_DATABASE_H
#define DB_DATABASE_H

#include <db/Catalog.h>
#include <db/BufferPool.h>

namespace db::Database {
    /** Return the buffer pool of the static Database instance */
    BufferPool &getBufferPool();

    /** Return the catalog of the static Database instance */
    Catalog &getCatalog();

    /**
     * Method used for testing -- create a new instance of the buffer pool and
     * return it
     */
    void resetBufferPool(int pages);

    /** reset the database, used for unit tests only. */
    void reset();
}


#endif
