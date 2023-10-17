#ifndef DB_TRANSACTIONID_H
#define DB_TRANSACTIONID_H

namespace db {
    enum class Permissions {
        READ_ONLY, READ_WRITE
    };

    class TransactionId {
        int id;
    public:
        TransactionId();

        operator int() const { return id; }
    };
}

#endif
