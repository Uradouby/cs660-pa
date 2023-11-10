#include <db/HeapFile.h>
#include <db/Page.h>
#include <db/PageId.h>
#include <db/HeapPage.h>
#include <db/BufferPool.h>
#include <db/Database.h>
#include <unistd.h>

using namespace db;

std::vector<Page *> HeapFile::insertTuple(TransactionId tid, Tuple &t) {
    // TODO pa3.3: some code goes here
}

std::vector<Page *> HeapFile::deleteTuple(TransactionId tid, Tuple &t) {
    // TODO pa3.3: some code goes here
}

void HeapFile::writePage(Page *p) {
    // TODO pa3.3: some code goes here
}
