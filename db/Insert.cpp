#include <db/Insert.h>
#include <db/Database.h>
#include <db/IntField.h>

using namespace db;

std::optional<Tuple> Insert::fetchNext() {
    // TODO pa3.3: some code goes here
}

Insert::Insert(TransactionId t, DbIterator *child, int tableId) {
    // TODO pa3.3: some code goes here
}

const TupleDesc &Insert::getTupleDesc() const {
    // TODO pa3.3: some code goes here
}

void Insert::open() {
    // TODO pa3.3: some code goes here
}

void Insert::close() {
    // TODO pa3.3: some code goes here
}

void Insert::rewind() {
    // TODO pa3.3: some code goes here
}

std::vector<DbIterator *> Insert::getChildren() {
    // TODO pa3.3: some code goes here
}

void Insert::setChildren(std::vector<DbIterator *> children) {
    // TODO pa3.3: some code goes here
}
