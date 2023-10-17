#include <db/SkeletonFile.h>
#include <stdexcept>
#include <string>

using namespace db;

SkeletonPageId::SkeletonPageId(int table, int page) {
    tableId = table;
    pgNo = page;
}

bool SkeletonPageId::operator==(const PageId &other) const {
    return pgNo == other.pageNumber() && tableId == other.getTableId();
}

int SkeletonPageId::getTableId() const {
    return tableId;
}

int SkeletonPageId::pageNumber() const {
    return pgNo;
}

const PageId &SkeletonPage::getId() const {
    return pid;
}

void *SkeletonPage::getPageData() const {
    return nullptr;
}

SkeletonPage::SkeletonPage(const PageId &pid) : pid(pid) {}

Page *SkeletonFile::readPage(const PageId &pid) {
    return new SkeletonPage(pid);
}

SkeletonFileIterator SkeletonFile::begin() const {
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " not implemented");
}

SkeletonFileIterator SkeletonFile::end() const {
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " not implemented");
}

std::vector<Page *> SkeletonFile::insertTuple(TransactionId tid, Tuple &t) {
    std::vector<Page *> pages;
    return pages;
}

std::vector<Page *> SkeletonFile::deleteTuple(TransactionId tid, Tuple &t) {
    std::vector<Page *> pages;
    return pages;
}

void SkeletonFile::writePage(Page *p) {
    writes++;
}
