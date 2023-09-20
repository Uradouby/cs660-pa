#include <db/HeapPageId.h>

using namespace db;

//
// HeapPageId
//

// TODO pa1.4: implement
HeapPageId::HeapPageId(int tableId, int pgNo) : tableId(tableId), pgNo(pgNo) {
}

int HeapPageId::getTableId() const {
    // TODO pa1.4: implement
}

int HeapPageId::pageNumber() const {
    // TODO pa1.4: implement
}

bool HeapPageId::operator==(const PageId &other) const {
    if (const auto *otherPageId = dynamic_cast<const HeapPageId *>(&other)) {
        // TODO pa1.4: implement
    }
    return false;
}
