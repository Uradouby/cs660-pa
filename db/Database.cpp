#include <db/Database.h>

using namespace db;

static Catalog catalog;
static BufferPool bufferpool(BufferPool::DEFAULT_PAGES);

BufferPool &Database::getBufferPool() { return bufferpool; }

Catalog &Database::getCatalog() { return catalog; }

void Database::resetBufferPool(int pages) {
    bufferpool.~BufferPool();
    new(&bufferpool) BufferPool(pages);
}

void Database::reset() {
    catalog.~Catalog();
    bufferpool.~BufferPool();
    new(&catalog) Catalog;
    new(&bufferpool) BufferPool(BufferPool::DEFAULT_PAGES);
}
