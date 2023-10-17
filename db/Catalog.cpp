#include <db/Catalog.h>
#include <db/Utility.h>
#include <db/TupleDesc.h>
#include <stdexcept>

using namespace db;

void Catalog::addTable(DbFile *file, const std::string &name, const std::string &pkeyField) {
    auto oldId = idToTable.find(file->getId());
    if (oldId != idToTable.end()) {
        idToTable.erase(oldId);
        nameToTable.erase(oldId->second->pkeyField);
    }
    auto oldName = nameToTable.find(name);
    if (oldName != nameToTable.end()) {
        nameToTable.erase(oldName);
        idToTable.erase(oldName->second->file->getId());
    }
    Table *table = new Table(file, name, pkeyField);
    idToTable[file->getId()] = table;
    nameToTable[name] = table;
}

int Catalog::getTableId(const std::string &name) const {
    auto it = nameToTable.find(name);
    if (it == nameToTable.end()) {
        throw std::invalid_argument("Table not found");
    }
    return it->second->file->getId();
}

const TupleDesc &Catalog::getTupleDesc(int tableid) const {
    auto it = idToTable.find(tableid);
    if (it == idToTable.end()) {
        throw std::runtime_error("Table not found");
    }
    return it->second->file->getTupleDesc();
}

DbFile *Catalog::getDatabaseFile(int tableid) const {
    auto it = idToTable.find(tableid);
    if (it == idToTable.end()) {
        throw std::runtime_error("Table not found");
    }
    return it->second->file;
}

std::string Catalog::getPrimaryKey(int tableid) const {
    auto it = idToTable.find(tableid);
    if (it == idToTable.end()) {
        throw std::runtime_error("Table not found");
    }
    return it->second->pkeyField;
}

std::string Catalog::getTableName(int id) const {
    auto it = idToTable.find(id);
    if (it == idToTable.end()) {
        throw std::runtime_error("Table not found");
    }
    return it->second->name;
}

void Catalog::clear() {
    idToTable.clear();
    nameToTable.clear();
}

void Catalog::addTable(DbFile *file) { addTable(file, Utility::generateUUID(), ""); }

Table::Table(DbFile *file, const std::string &name, const std::string &pkeyField) : file(file), name(name), pkeyField(pkeyField) {}
