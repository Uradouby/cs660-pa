#include <db/HeapFile.h>
#include <db/TupleDesc.h>
#include <db/Page.h>
#include <db/PageId.h>
#include <db/HeapPage.h>
#include <stdexcept>
#include <sys/stat.h>
#include <fcntl.h>

using namespace db;

//
// HeapFile
//

// TODO pa1.5: implement
HeapFile::HeapFile(const char *fname, const TupleDesc &td) : td(td) {
}

int HeapFile::getId() const {
    // TODO pa1.5: implement
}

const TupleDesc &HeapFile::getTupleDesc() const {
    // TODO pa1.5: implement
}

Page *HeapFile::readPage(const PageId &pid) {
    // TODO pa1.5: implement
}

int HeapFile::getNumPages() {
    // TODO pa1.5: implement
}

HeapFileIterator HeapFile::begin() const {
    // TODO pa1.5: implement
}

HeapFileIterator HeapFile::end() const {
    // TODO pa1.5: implement
}

//
// HeapFileIterator
//

// TODO pa1.5: implement
HeapFileIterator::HeapFileIterator(/* TODO pa1.5: add parameters */) {
}

bool HeapFileIterator::operator!=(const HeapFileIterator &other) const {
    // TODO pa1.5: implement
}

Tuple &HeapFileIterator::operator*() const {
    // TODO pa1.5: implement
}

HeapFileIterator &HeapFileIterator::operator++() {
    // TODO pa1.5: implement
}
