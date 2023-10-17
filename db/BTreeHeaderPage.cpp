#include <db/BTreeHeaderPage.h>
#include <db/Database.h>
#include <cassert>

using namespace db;

int BTreeHeaderPage::getHeaderSize() {
    // pointerBytes: nextPage and prevPage pointers
    int pointerBytes = 2 * sizeof(int);
    return Database::getBufferPool().getPageSize() - pointerBytes;
}

BTreeHeaderPage::BTreeHeaderPage(const BTreePageId *id, uint8_t *data) {
    pid = id;
    numSlots = getNumSlots();

    // Read the next and prev pointers
    int *int_data = (int *) data;
    nextPage = int_data[0];
    prevPage = int_data[1];
    // allocate and read the header slots of this page
    size_t header_size = getHeaderSize();
    header = new uint8_t[header_size];
    memcpy(header, int_data + 2, header_size);
}

void BTreeHeaderPage::init() {
    memset(header, 0xFF, getHeaderSize());
}

int BTreeHeaderPage::getNumSlots() {
    return getHeaderSize() * 8;
}

const BTreePageId &BTreeHeaderPage::getId() const {
    return *pid;
}

void *BTreeHeaderPage::getPageData() const {
    void *data = createEmptyPageData();
    int *int_data = (int *) data;
    size_t header_size = getHeaderSize();
    memcpy(int_data, &nextPage, sizeof(int));
    memcpy(int_data + 1, &prevPage, sizeof(int));
    memcpy(int_data + 2, header, header_size);
    return data;
}

void *BTreeHeaderPage::createEmptyPageData() {
    int len = Database::getBufferPool().getPageSize();
    return new uint8_t[len]{}; //all 0
}

BTreePageId *BTreeHeaderPage::getPrevPageId() {
    if (prevPage == 0) {
        return nullptr;
    }
    return new BTreePageId(pid->getTableId(), prevPage, BTreePageType::HEADER);
}

BTreePageId *BTreeHeaderPage::getNextPageId() {
    if (nextPage == 0) {
        return nullptr;
    }
    return new BTreePageId(pid->getTableId(), nextPage, BTreePageType::HEADER);
}

void BTreeHeaderPage::setPrevPageId(const BTreePageId *id) {
    if (id == nullptr) {
        prevPage = 0;
    } else {
        // table id mismatch in setPrevPageId
        assert(id->getTableId() == pid->getTableId());
        // prevPage must be a header page
        assert(id->getType() == BTreePageType::HEADER);
        prevPage = id->pageNumber();
    }
}

void BTreeHeaderPage::setNextPageId(const BTreePageId *id) {
    if (id == nullptr) {
        nextPage = 0;
    } else {
        // table id mismatch in setPrevPageId
        assert(id->getTableId() == pid->getTableId());
        // prevPage must be a header page
        assert(id->getType() == BTreePageType::HEADER);
        nextPage = id->pageNumber();
    }
}

bool BTreeHeaderPage::isSlotUsed(int i) {
    int headerbit = i % 8;
    int headerbyte = (i - headerbit) / 8;
    return (header[headerbyte] & (1 << headerbit)) != 0;
}

void BTreeHeaderPage::markSlotUsed(int i, bool value) {
    int headerbit = i % 8;
    int headerbyte = (i - headerbit) / 8;

    if (value) {
        header[headerbyte] |= 1 << headerbit;
    } else {
        header[headerbyte] &= (0xFF ^ (1 << headerbit));
    }
}

int BTreeHeaderPage::getEmptySlot() {
    size_t header_size = getHeaderSize();
    for (int i = 0; i < header_size; i++) {
        for (int j = 0; j < 8; j++) {
            if (!isSlotUsed(i * 8 + j)) {
                return i * 8 + j;
            }
        }
    }
    return -1;
}
