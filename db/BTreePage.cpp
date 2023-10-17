#include <db/BTreePage.h>
#include <db/Database.h>
#include <db/Catalog.h>
#include <db/BufferPool.h>
#include <cassert>

using namespace db;

BTreePage::BTreePage(const BTreePageId &id, int key) : pid(id), keyField(key), parent(0),
                                                           td(Database::getCatalog().getTupleDesc(id.getTableId())) {}

void *BTreePage::createEmptyPageData() {
    int len = Database::getBufferPool().getPageSize();
    return new uint8_t[len]{}; //all 0
}

void BTreePage::setParentId(const BTreePageId *id) {
    // parent id must not be null
    assert(id != nullptr);
    // table id mismatch in setParentId
    assert(id->getTableId() == pid.getTableId());
    // parent must be an internal node or root pointer
    assert(id->getType() == BTreePageType::INTERNAL || id->getType() == BTreePageType::ROOT_PTR);
    if (id->getType() == BTreePageType::ROOT_PTR) {
        parent = 0;
    } else {
        parent = id->pageNumber();
    }
}

const BTreePageId &BTreePage::getId() const {
    return pid;
}

BTreePageId *BTreePage::getParentId() {
    if (parent == 0) {
        return new BTreePageId(pid.getTableId(), 0, BTreePageType::ROOT_PTR);
    }
    return new BTreePageId(pid.getTableId(), parent, BTreePageType::INTERNAL);
}
