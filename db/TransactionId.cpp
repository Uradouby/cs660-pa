#include <db/TransactionId.h>

using namespace db;

static int nextTransactionId = 0;

TransactionId::TransactionId() : id(nextTransactionId++) {}
