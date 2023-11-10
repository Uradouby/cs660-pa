#include <db/HeapFile.h>
#include <db/Page.h>
#include <db/PageId.h>
#include <db/HeapPage.h>
#include <db/BufferPool.h>
#include <db/Database.h>
#include <unistd.h>

using namespace db;

std::vector<Page *> HeapFile::insertTuple(TransactionId tid, Tuple &t) {
    BufferPool &bufferPool = Database::getBufferPool();
    HeapPage *page = nullptr;
    for (int i = 0; i < numPages; i++) {
        auto *pid = new HeapPageId(tableid, i);
        auto *currPage = dynamic_cast<HeapPage *>(bufferPool.getPage(pid));
        if (currPage->getNumEmptySlots() > 0) {
            page = dynamic_cast<HeapPage *>(bufferPool.getPage(pid));
            break;
        }
    }
    if (page == nullptr) {
        page = new HeapPage(HeapPageId(tableid, numPages), static_cast<uint8_t *>(HeapPage::createEmptyPageData()));
    }
    page->insertTuple(&t);
    return {page};
}

std::vector<Page *> HeapFile::deleteTuple(TransactionId tid, Tuple &t) {
    auto *page = dynamic_cast<HeapPage *>(Database::getBufferPool().getPage(t.getRecordId()->getPageId()));
    page->deleteTuple(&t);
    return {page};
}

void HeapFile::writePage(Page *p) {
    auto data = p->getPageData();
    auto offset = p->getId().pageNumber() * Database::getBufferPool().getPageSize();
    pwrite(fd, data, Database::getBufferPool().getPageSize(), offset);
}
