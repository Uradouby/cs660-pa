#include <db/BTreePageId.h>

using namespace db;

bool BTreePageId::operator==(const PageId &other) const {
    if (const auto *otherPageId = dynamic_cast<const BTreePageId *>(&other)) {
        return tableId == otherPageId->tableId && pgNo == otherPageId->pgNo && type == otherPageId->type;
    }
    return false;
}

size_t std::hash<db::BTreePageId>::operator()(const db::BTreePageId &pid) const {
    return pid.getTableId() ^ pid.pageNumber();
}
