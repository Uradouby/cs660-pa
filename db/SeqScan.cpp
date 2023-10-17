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
    tableid = tabid;
    alias = tableAlias;
    tableName = Database::getCatalog().getTableName(tableid);
}

const TupleDesc &SeqScan::getTupleDesc() const {
    return Database::getCatalog().getTupleDesc(tableid);
}

SeqScan::iterator SeqScan::begin() const {
    DbFile *file = Database::getCatalog().getDatabaseFile(tableid);
    if (auto heapFile = dynamic_cast<HeapFile *>(file)) {
        return heapFile->begin();
    }
    throw std::runtime_error("can't iterate");
}

SeqScan::iterator SeqScan::end() const {
    DbFile *file = Database::getCatalog().getDatabaseFile(tableid);
    if (auto heapFile = dynamic_cast<HeapFile *>(file)) {
        return heapFile->end();
    }
    throw std::runtime_error("can't iterate");
}

SeqScan::SeqScan(int tableid) : SeqScan(tableid, Database::getCatalog().getTableName(tableid)) {}
