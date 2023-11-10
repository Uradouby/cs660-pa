#include <db/SeqScan.h>
#include <db/Database.h>
#include <db/Catalog.h>
#include <db/BufferPool.h>

using namespace db;

SeqScan::SeqScan(int tableid, const std::string &tableAlias) {
    reset(tableid, tableAlias);
}

std::string SeqScan::getTableName() const {
    return tableName;
}

std::string SeqScan::getAlias() const {
    return alias;
}

void SeqScan::reset(int tabid, const std::string &tableAlias) {
    itopt = std::nullopt;
    tableid = tabid;
    alias = tableAlias;
    tableName = Database::getCatalog().getTableName(tableid);
}

const TupleDesc &SeqScan::getTupleDesc() const {
    return Database::getCatalog().getTupleDesc(tableid);
}

SeqScan::SeqScan(int tableid) : SeqScan(tableid, Database::getCatalog().getTableName(tableid)) {}

void SeqScan::open() {
    DbFile *file = Database::getCatalog().getDatabaseFile(tableid);
    if (auto heapFile = dynamic_cast<HeapFile *>(file)) {
        itopt = heapFile->begin();
    } else {
        throw std::runtime_error("can't open");
    }
}

bool SeqScan::hasNext() {
    DbFile *file = Database::getCatalog().getDatabaseFile(tableid);
    if (auto heapFile = dynamic_cast<HeapFile *>(file)) {
        auto it = itopt.value();
        return it != heapFile->end();
    }
    throw std::runtime_error("can't next");
}

Tuple SeqScan::next() {
    auto it = itopt.value();
    auto tup = *it;
    itopt = ++it;
    return tup;
}

void SeqScan::rewind() {
    close();
    open();
}

void SeqScan::close() {
    itopt = std::nullopt;
}
