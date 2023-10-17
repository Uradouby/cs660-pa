#include <db/HeapFile.h>
#include <db/Database.h>
#include <db/TupleDesc.h>
#include <db/Page.h>
#include <db/PageId.h>
#include <db/HeapPage.h>
#include <stdexcept>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace db;

//
// HeapFile
//

HeapFile::HeapFile(const char *fname, const TupleDesc &td) : td(td) {
    tableid = std::hash<std::string>{}(fname);
    fd = open(fname, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        throw std::runtime_error("open");
    }
    struct stat st{};
    if (fstat(fd, &st) == -1) {
        throw std::runtime_error("fstat");
    }
    numPages = st.st_size / Database::getBufferPool().getPageSize();
}

int HeapFile::getId() const {
    return tableid;
}

const TupleDesc &HeapFile::getTupleDesc() const {
    return td;
}

Page *HeapFile::readPage(const PageId &pid) {
    auto page_size = Database::getBufferPool().getPageSize();
    uint8_t *data = new uint8_t[page_size];
    const HeapPageId *hpid = dynamic_cast<const HeapPageId *>(&pid);
    pread(fd, data, page_size, hpid->pageNumber() * page_size);
    HeapPage *page = new HeapPage(*hpid, data);
    return page;
}

int HeapFile::getNumPages() const {
    return numPages;
}

HeapFileIterator HeapFile::begin() const {
    return {getId(), getNumPages()};
}

HeapFileIterator HeapFile::end() const {
    return {getId(), getNumPages(), true};
}

std::vector<Page *> HeapFile::insertTuple(TransactionId tid, Tuple &t) {
    std::vector<Page *> pages;
    return pages;
}

std::vector<Page *> HeapFile::deleteTuple(TransactionId tid, Tuple &t) {
    std::vector<Page *> pages;
    return pages;
}

void HeapFile::writePage(Page *p) {
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " not implemented");
}

//
// HeapFileIterator
//

HeapFileIterator::HeapFileIterator(int tableid, int numPages, bool end) : hpid(tableid, 0) {
    this->numPages = numPages;
    this->end = end;
    int pid;
    if (end) {
        pid = numPages;
    } else {
        pid = 0;
    }
    hpid = {tableid, pid};
    if (end) {
    } else {
        auto p = Database::getBufferPool().getPage(&hpid);
        page = dynamic_cast<HeapPage *>(p);
        if (!page) {
            throw std::runtime_error("dynamic_cast");
        }
        auto hpit = page->begin();
        it = new HeapPageIterator(hpit);
    }
}

bool HeapFileIterator::operator!=(const HeapFileIterator &other) const {
    return hpid != other.hpid || end != other.end;
}

Tuple &HeapFileIterator::operator*() const {
    return *(*it);
}

HeapFileIterator &HeapFileIterator::operator++() {
    ++(*it);
    if (*it != page->end()) {
        return *this;
    }
    delete it;
    hpid = {hpid.getTableId(), hpid.pageNumber() + 1};
    if (hpid.pageNumber() >= numPages) {
        end = true;
        return *this;
    }
    auto p = Database::getBufferPool().getPage(&hpid);
    page = dynamic_cast<HeapPage *>(p);
    if (!page) {
        throw std::runtime_error("dynamic_cast");
    }
    auto hpit = page->begin();
    it = new HeapPageIterator(hpit);
    return *this;
}
