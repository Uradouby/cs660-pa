#include <db/Catalog.h>
#include <db/TupleDesc.h>
#include <stdexcept>

using namespace db;

void Catalog::addTable(DbFile *file, const std::string &name, const std::string &pkeyField) {
    // TODO pa1.2: implement
}

int Catalog::getTableId(const std::string &name) const {
    // TODO pa1.2: implement
}

const TupleDesc &Catalog::getTupleDesc(int tableid) const {
    // TODO pa1.2: implement
}

DbFile *Catalog::getDatabaseFile(int tableid) const {
    // TODO pa1.2: implement
}

std::string Catalog::getPrimaryKey(int tableid) const {
    // TODO pa1.2: implement
}

std::string Catalog::getTableName(int id) const {
    // TODO pa1.2: implement
}

void Catalog::clear() {
    // TODO pa1.2: implement
}
