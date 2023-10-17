#include <db/HeapPageId.h>

using namespace db;

//
// HeapPageId
//

HeapPageId::HeapPageId(int tableId, int pgNo) : tableId(tableId), pgNo(pgNo) {
}

int HeapPageId::getTableId() const {
    return tableId;
}

int HeapPageId::pageNumber() const {
    return pgNo;
}

bool HeapPageId::operator==(const PageId &other) const {
    if (const auto *otherPageId = dynamic_cast<const HeapPageId *>(&other)) {
        return tableId == otherPageId->tableId && pgNo == otherPageId->pgNo;
    }
    return false;
}
