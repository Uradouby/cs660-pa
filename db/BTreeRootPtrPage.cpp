#include <db/BTreeRootPtrPage.h>
#include <cstring>
#include <cstdint>
#include <cassert>

using namespace db;

void *BTreeRootPtrPage::getPageData() const {
    int *data = (int *) createEmptyPageData();
    data[0] = root;
    data[1] = header;
    memcpy(data + 2, &rootCategory, sizeof(rootCategory));
    return data;
}

void *BTreeRootPtrPage::createEmptyPageData() {
    return new uint8_t[PAGE_SIZE]{}; //all 0
}

BTreePageId *BTreeRootPtrPage::getRootId() {
    if (root == 0) {
        return nullptr;
    }
    return new BTreePageId(pid->getTableId(), root, rootCategory);
}

void BTreeRootPtrPage::setRootId(const BTreePageId *id) {
    if (id == nullptr) {
        root = 0;
    } else {
        // table id mismatch in setRootId
        assert(id->getTableId() == pid->getTableId());
        // root must be an internal node or leaf node
        assert(id->getType() == BTreePageType::INTERNAL || id->getType() == BTreePageType::LEAF);
        root = id->pageNumber();
        rootCategory = id->getType();
    }
}

BTreePageId *BTreeRootPtrPage::getHeaderId() const {
    if (header == 0) {
        return nullptr;
    }
    return new BTreePageId(pid->getTableId(), header, BTreePageType::HEADER);
}

void BTreeRootPtrPage::setHeaderId(const BTreePageId *id) {
    if (id == nullptr) {
        header = 0;
    } else {
        // table id mismatch in setHeaderId
        assert(id->getTableId() == pid->getTableId());
        // header must be of type HEADER
        assert(id->getType() == BTreePageType::HEADER);
        header = id->pageNumber();
    }
}

int BTreeRootPtrPage::getPageSize() {
    return PAGE_SIZE;
}

BTreeRootPtrPage::BTreeRootPtrPage(const BTreePageId *id, void *data) {
    this->pid = id;

    // read in the root pointer
    int *root_p = (int *) data;
    root = root_p[0];
    // read in the header pointer
    header = root_p[1];
    rootCategory = *(BTreePageType *) (root_p + 2);
}

const PageId &BTreeRootPtrPage::getId() const {
    return *pid;
}

BTreePageId *BTreeRootPtrPage::getId(int tableid) {
    return new BTreePageId(tableid, 0, BTreePageType::ROOT_PTR);
}
