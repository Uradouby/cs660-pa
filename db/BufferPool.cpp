#include <db/BufferPool.h>
#include <db/Database.h>

using namespace db;

// TODO pa1.3: implement
BufferPool::BufferPool(int numPages) {
}

Page *BufferPool::getPage(const TransactionId &tid, PageId *pid) {
    // TODO pa1.3: implement
}

void BufferPool::evictPage() {
    // do nothing for now
}
