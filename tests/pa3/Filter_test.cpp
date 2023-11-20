#include <gtest/gtest.h>
#include <db/Database.h>
#include <db/Utility.h>
#include <db/HeapFile.h>
#include <db/SeqScan.h>
#include <db/IntField.h>
#include <db/Filter.h>

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

TEST(FilterTest, NE) {
    db::TupleDesc td = db::Utility::getTupleDesc(3);
    db::HeapFile table("table.dat", td);
    db::Database::getCatalog().addTable(&table, "t1");

    db::SeqScan ss1(table.getId(), "s1");
    db::Predicate pred(1, db::Predicate::Op::NOT_EQUALS, new db::IntField(30));
    db::Filter f1(pred, &ss1);
    EXPECT_EQ(count(&f1), 345);
}

TEST(FilterTest, EQ) {
    db::TupleDesc td = db::Utility::getTupleDesc(3);
    db::HeapFile table("table.dat", td);
    db::Database::getCatalog().addTable(&table, "t1");

    db::SeqScan ss1(table.getId(), "s1");
    db::Predicate pred(1, db::Predicate::Op::EQUALS, new db::IntField(30));
    db::Filter f1(pred, &ss1);
    EXPECT_EQ(count(&f1), 5);
}

TEST(FilterTest, GT) {
    db::TupleDesc td = db::Utility::getTupleDesc(3);
    db::HeapFile table("table.dat", td);
    db::Database::getCatalog().addTable(&table, "t1");

    db::SeqScan ss1(table.getId(), "s1");
    db::Predicate pred(1, db::Predicate::Op::GREATER_THAN, new db::IntField(30));
    db::Filter f1(pred, &ss1);
    EXPECT_EQ(count(&f1), 195);
}

TEST(FilterTest, GTE) {
    db::TupleDesc td = db::Utility::getTupleDesc(3);
    db::HeapFile table("table.dat", td);
    db::Database::getCatalog().addTable(&table, "t1");

    db::SeqScan ss1(table.getId(), "s1");
    db::Predicate pred(1, db::Predicate::Op::GREATER_THAN_OR_EQ, new db::IntField(30));
    db::Filter f1(pred, &ss1);
    EXPECT_EQ(count(&f1), 200);
}

TEST(FilterTest, LT) {
    db::TupleDesc td = db::Utility::getTupleDesc(3);
    db::HeapFile table("table.dat", td);
    db::Database::getCatalog().addTable(&table, "t1");

    db::SeqScan ss1(table.getId(), "s1");
    db::Predicate pred(1, db::Predicate::Op::LESS_THAN, new db::IntField(30));
    db::Filter f1(pred, &ss1);
    EXPECT_EQ(count(&f1), 150);
}

TEST(FilterTest, LTE) {
    db::TupleDesc td = db::Utility::getTupleDesc(3);
    db::HeapFile table("table.dat", td);
    db::Database::getCatalog().addTable(&table, "t1");

    db::SeqScan ss1(table.getId(), "s1");
    db::Predicate pred(1, db::Predicate::Op::LESS_THAN_OR_EQ, new db::IntField(30));
    db::Filter f1(pred, &ss1);
    EXPECT_EQ(count(&f1), 155);
}
