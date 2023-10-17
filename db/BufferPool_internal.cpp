#include <db/BufferPool.h>

#include <db/Database.h>
#include <db/Page.h>

using namespace db;

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
