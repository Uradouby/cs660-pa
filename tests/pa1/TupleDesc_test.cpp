#include <gtest/gtest.h>
#include <db/TupleDesc.h>
#include <db/Utility.h>
#include <db/Type.h>

bool combinedStringArrays(const db::TupleDesc &td1, const db::TupleDesc &td2, const db::TupleDesc &combined) {
    for (int i = 0; i < td1.numFields(); i++) {
        if (!((td1.getFieldName(i).empty() && combined.getFieldName(i).empty()) ||
              td1.getFieldName(i) == combined.getFieldName(i))) {
            return false;
        }
    }

    for (int i = td1.numFields(); i < td1.numFields() + td2.numFields(); i++) {
        if (!((td2.getFieldName(i - td1.numFields()).empty() && combined.getFieldName(i).empty()) ||
              td2.getFieldName(i - td1.numFields()) == combined.getFieldName(i))) {
            return false;
        }
    }

    return true;
}

TEST(TupleDescTest, Combine) {
    const db::TupleDesc &td1 = db::Utility::getTupleDesc(1, "td1");
    const db::TupleDesc &td2 = db::Utility::getTupleDesc(2, "td2");
    db::TupleDesc td3 = db::TupleDesc::merge(td1, td2);
    EXPECT_EQ(3, td3.numFields());
    EXPECT_EQ(3 * db::Types::getLen(db::Types::INT_TYPE), td3.getSize());

    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(db::Types::INT_TYPE, td3.getFieldType(i));
    }

    EXPECT_TRUE(combinedStringArrays(td1, td2, td3));

    td3 = db::TupleDesc::merge(td2, td1);
    EXPECT_EQ(3, td3.numFields());
    EXPECT_EQ(3 * db::Types::getLen(db::Types::INT_TYPE), td3.getSize());

    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(db::Types::INT_TYPE, td3.getFieldType(i));
    }

    EXPECT_TRUE(combinedStringArrays(td2, td1, td3));

    td3 = db::TupleDesc::merge(td2, td2);
    EXPECT_EQ(4, td3.numFields());
    EXPECT_EQ(4 * db::Types::getLen(db::Types::INT_TYPE), td3.getSize());

    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(db::Types::INT_TYPE, td3.getFieldType(i));
    }

    EXPECT_TRUE(combinedStringArrays(td2, td2, td3));
}

TEST(TupleDescTest, GetType) {
    int lengths[] = {1, 2, 1000};

    for (int len: lengths) {
        db::TupleDesc td = db::Utility::getTupleDesc(len);
        for (int i = 0; i < len; ++i) {
            EXPECT_EQ(db::Types::INT_TYPE, td.getFieldType(i));
        }
    }
}

TEST(TupleDescTest, NameToId) {
    int lengths[] = {1, 2, 1000};
    std::string prefix = "test";

    for (int len: lengths) {
        db::TupleDesc named_td = db::Utility::getTupleDesc(len, prefix);
        for (int i = 0; i < len; ++i) {
            EXPECT_EQ(i, named_td.fieldNameToIndex(prefix + std::to_string(i)));
        }

        EXPECT_THROW(named_td.fieldNameToIndex("foo"), std::invalid_argument);

        db::TupleDesc unnamed_td = db::Utility::getTupleDesc(len);
        EXPECT_THROW(unnamed_td.fieldNameToIndex(prefix), std::invalid_argument);
    }
}

TEST(TupleDescTest, GetSize) {
    int lengths[] = {1, 2, 1000};

    for (int len: lengths) {
        db::TupleDesc td = db::Utility::getTupleDesc(len);
        EXPECT_EQ(len * db::Types::getLen(db::Types::INT_TYPE), td.getSize());
    }
}

TEST(TupleDescTest, NumFields) {
    int lengths[] = {1, 2, 1000};

    for (int len: lengths) {
        db::TupleDesc td = db::Utility::getTupleDesc(len);
        EXPECT_EQ(len, td.numFields());
    }
}

TEST(TupleDescTest, Equals) {
    std::vector<db::Types::Type> inttypes1 = {db::Types::INT_TYPE,};
    db::TupleDesc singleInt(inttypes1);
    std::vector<db::Types::Type> inttypes2 = {db::Types::INT_TYPE,};
    db::TupleDesc singleInt2(inttypes2);
    std::vector<db::Types::Type> intstringtypes = {db::Types::INT_TYPE, db::Types::STRING_TYPE};
    db::TupleDesc intString(intstringtypes);

    EXPECT_EQ(singleInt, singleInt);
    EXPECT_EQ(singleInt, singleInt2);
    EXPECT_EQ(singleInt2, singleInt);
    EXPECT_EQ(intString, intString);

    EXPECT_NE(singleInt, intString);
    EXPECT_NE(singleInt2, intString);
    EXPECT_NE(intString, singleInt);
    EXPECT_NE(intString, singleInt2);
}

TEST(TupleDescTest, Iterate) {
    std::vector<db::Types::Type> types = {db::Types::INT_TYPE, db::Types::STRING_TYPE, db::Types::INT_TYPE,
                                                   db::Types::INT_TYPE, db::Types::STRING_TYPE};
    std::vector<std::string> names = {"int1", "string1", "int2", "int3", "string2"};
    db::TupleDesc td(types, names);

    auto td_it = td.begin();
    auto types_it = types.begin();
    auto names_it = names.begin();
    while (td_it != td.end()) {
        auto item = *td_it;
        EXPECT_EQ(item.fieldType, *types_it);
        EXPECT_EQ(item.fieldName, *names_it);
        ++types_it;
        ++names_it;
        ++td_it;
    }
}
