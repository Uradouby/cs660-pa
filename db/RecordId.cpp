#include <db/RecordId.h>
#include <stdexcept>

using namespace db;

//
// RecordId
//

RecordId::RecordId(const PageId *pid, int tupleno) : pid(pid), tupleno(tupleno) {
}

bool RecordId::operator==(const RecordId &other) const {
    return *pid == *other.pid && tupleno == other.tupleno;
}

const PageId *RecordId::getPageId() const {
    return pid;
}

int RecordId::getTupleno() const {
    return tupleno;
}

//
// RecordId hash function
//

std::size_t std::hash<RecordId>::operator()(const RecordId &r) const {
    return std::hash<PageId>()(*r.getPageId()) ^ std::hash<int>()(r.getTupleno());
}
