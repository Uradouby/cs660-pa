#include <gtest/gtest.h>
#include <db/Tuple.h>
#include <db/TupleDesc.h>
#include <db/Utility.h>
#include <db/IntField.h>
#include <db/StringField.h>

TEST(TupleTests, ModifyFields) {
    db::TupleDesc td = db::Utility::getTupleDesc(2);
    db::IntField one(1);
    db::IntField zero(0);
    db::IntField one2(1);
    db::IntField zero2(0);
    db::Tuple tup(td);
    tup.setField(0, &one);
    tup.setField(1, &zero);

    EXPECT_EQ(one2, tup.getField(0));
    EXPECT_EQ(zero2, tup.getField(1));

    db::IntField negative(-1);
    db::IntField number(37);
    tup.setField(0, &negative);
    tup.setField(1, &number);
    db::IntField negative2(-1);
    db::IntField number2(37);
    EXPECT_EQ(negative2, tup.getField(0));
    EXPECT_EQ(number2, tup.getField(1));
}

TEST(TupleTests, GetTupleDesc) {
    const db::TupleDesc td = db::Utility::getTupleDesc(5);
    db::Tuple tup(td);
    EXPECT_EQ(td, tup.getTupleDesc());
}

TEST(TupleTests, ModifyRecordId) {
    db::Tuple tup(db::Utility::getTupleDesc(1));
    db::RecordId rid;
    tup.setRecordId(&rid);
    EXPECT_EQ(&rid, tup.getRecordId());
}


TEST(TupleTests, Iterate) {
    std::vector<db::Types::Type> types = {db::Types::INT_TYPE, db::Types::STRING_TYPE, db::Types::INT_TYPE,
                                          db::Types::INT_TYPE, db::Types::STRING_TYPE};
    std::vector<std::string> names = {"int1", "string1", "int2", "int3", "string2"};
    db::TupleDesc td(types, names);

    db::Tuple tup(td);
    db::IntField field0(0);
    db::StringField field1("hello");
    db::IntField field2(2);
    db::IntField field3(3);
    db::StringField field4("world");
    tup.setField(0, &field0);
    tup.setField(1, &field1);
    tup.setField(2, &field2);
    tup.setField(3, &field3);
    tup.setField(4, &field4);
    int i = 0;
    for (const auto &item: tup) {
        EXPECT_EQ(item->getType(), types[i]);
        if (i == 0 || i == 2 || i == 3) {
            if (const auto *field = dynamic_cast<const db::IntField *>(&tup.getField(i))) {
                EXPECT_EQ(field->getValue(), i);
            } else {
                FAIL();
            }
        } else if (i == 1) {
            if (const auto *field = dynamic_cast<const db::StringField *>(&tup.getField(i))) {
                EXPECT_EQ(field->getValue(), "hello");
            } else {
                FAIL();
            }
        } else {
            if (const auto *field = dynamic_cast<const db::StringField *>(&tup.getField(i))) {
                EXPECT_EQ(field->getValue(), "world");
            } else {
                FAIL();
            }
        }
        i++;
    }
}
