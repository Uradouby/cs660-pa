#include <db/BufferPool.h>
#include <db/Database.h>

using namespace db;

void BufferPool::evictPage() {
    auto it = pages.begin();
    if (it != pages.end()) {
        flushPage(it->first);
        pages.erase(it);
    }
}

void BufferPool::flushAllPages() {
    for (const auto &item: pages) {
        flushPage(item.first);
    }
}

void BufferPool::discardPage(const PageId *pid) {
    auto it = pages.find(pid);
    if (it != pages.end()) {
        pages.erase(it);
    }
}

void BufferPool::flushPage(const PageId *pid) {
    auto it = pages.find(pid);
    if (it != pages.end() && it->second->isDirty().has_value()) {
        it->second->markDirty(std::nullopt);
        Database::getCatalog().getDatabaseFile(pid->getTableId())->writePage(it->second);
    }
}

void BufferPool::flushPages(const TransactionId &tid) {
    for (const auto &item: pages) {
        if (item.second->isDirty() == tid) {
            flushPage(item.first);
        }
    }
}

void BufferPool::insertTuple(const TransactionId &tid, int tableId, Tuple *t) {
    auto f = Database::getCatalog().getDatabaseFile(tableId);
    auto dirtypages = f->insertTuple(tid, *t);
    for (auto page: dirtypages) {
        page->markDirty(tid);
        const PageId *pid = &page->getId();
        if (pages.size() >= numPages && pages.find(pid) == pages.end()) {
            evictPage();
        }
        pages[pid] = page;
    }
}

void BufferPool::deleteTuple(const TransactionId &tid, Tuple *t) {
    int tableId = t->getRecordId()->getPageId()->getTableId();
    auto f = Database::getCatalog().getDatabaseFile(tableId);
    auto dirtypages = f->insertTuple(tid, *t);
    for (auto page: dirtypages) {
        page->markDirty(tid);
        const PageId *pid = &page->getId();
        if (pages.size() >= numPages && pages.find(pid) == pages.end()) {
            evictPage();
        }
        pages[pid] = page;
    }
}

Page *BufferPool::getPage(const PageId *pid) {
    auto it = pages.find(pid);
    if (it != pages.end()) {
        return it->second;
    }
    if (pages.size() >= numPages) {
        evictPage();
    }
    Page *page = Database::getCatalog().getDatabaseFile(pid->getTableId())->readPage(*pid);
    pages[pid] = page;
    return page;
}

const PagesMap &BufferPool::getPages() const { return pages; }

const int &BufferPool::getNumPages() const { return numPages; }
