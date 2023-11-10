#include <db/Delete.h>
#include <db/BufferPool.h>
#include <db/IntField.h>
#include <db/Database.h>

using namespace db;

Delete::Delete(TransactionId t, DbIterator *child) {
    // TODO pa3.4: some code goes here
}

const TupleDesc &Delete::getTupleDesc() const {
    // TODO pa3.4: some code goes here
}

void Delete::open() {
    // TODO pa3.4: some code goes here
}

void Delete::close() {
    // TODO pa3.4: some code goes here
}

void Delete::rewind() {
    // TODO pa3.4: some code goes here
}

std::vector<DbIterator *> Delete::getChildren() {
    // TODO pa3.4: some code goes here
}

void Delete::setChildren(std::vector<DbIterator *> children) {
    // TODO pa3.4: some code goes here
}

std::optional<Tuple> Delete::fetchNext() {
    // TODO pa3.4: some code goes here
}
