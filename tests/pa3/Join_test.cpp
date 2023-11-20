#include <gtest/gtest.h>
#include <db/Database.h>
#include <db/Utility.h>
#include <db/HeapFile.h>
#include <db/SeqScan.h>
#include <db/IntField.h>
#include <db/Join.h>

static int count(db::DbIterator *it) {
    int i = 0;
    it->open();
    while (it->hasNext()) {
        auto tup = it->next();
        ++i;
        std::cout << i << ' ' << tup.to_string() << std::endl;
    }
    it->close();
    return i;
}

TEST(JoinTest, test) {
    db::TupleDesc td = db::Utility::getTupleDesc(3);

    db::HeapFile table("table.dat", td);
    db::Database::getCatalog().addTable(&table, "t1");
    db::SeqScan ss1(table.getId(), "s1");
    db::SeqScan ss2(table.getId(), "s2");

    db::JoinPredicate pred(0, db::Predicate::Op::GREATER_THAN_OR_EQ, 1);
    db::Join join(&pred, &ss1, &ss2);
    EXPECT_EQ(count(&join), 5250);
}
