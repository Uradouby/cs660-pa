#include <gtest/gtest.h>
#include <db/Database.h>
#include <db/HeapPage.h>
#include <db/HeapPageId.h>
#include <db/Utility.h>
#include <db/Tuple.h>
#include <db/IntField.h>
#include <db/SkeletonFile.h>

int EXAMPLE_VALUES[][2] = {{31933, 862},
                           {29402, 56883},
                           {1468,  5825},
                           {17876, 52278},
                           {6350,  36090},
                           {34784, 43771},
                           {28617, 56874},
                           {19209, 23253},
                           {56462, 24979},
                           {51440, 56685},
                           {3596,  62307},
                           {45569, 2719},
                           {22064, 43575},
                           {42812, 44947},
                           {22189, 19724},
                           {33549, 36554},
                           {9086,  53184},
                           {42878, 33394},
                           {62778, 21122},
                           {17197, 16388}};

uint8_t EXAMPLE_DATA[4096] = {
        // Page header 64B
        0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // 20 tuples
        0xbd, 0x7c, 0x00, 0x00, 0x5e, 0x03, 0x00, 0x00,
        0xda, 0x72, 0x00, 0x00, 0x33, 0xde, 0x00, 0x00,
        0xbc, 0x05, 0x00, 0x00, 0xc1, 0x16, 0x00, 0x00,
        0xd4, 0x45, 0x00, 0x00, 0x36, 0xcc, 0x00, 0x00,
        0xce, 0x18, 0x00, 0x00, 0xfa, 0x8c, 0x00, 0x00,
        0xe0, 0x87, 0x00, 0x00, 0xfb, 0xaa, 0x00, 0x00,
        0xc9, 0x6f, 0x00, 0x00, 0x2a, 0xde, 0x00, 0x00,
        0x09, 0x4b, 0x00, 0x00, 0xd5, 0x5a, 0x00, 0x00,
        0x8e, 0xdc, 0x00, 0x00, 0x93, 0x61, 0x00, 0x00,
        0xf0, 0xc8, 0x00, 0x00, 0x6d, 0xdd, 0x00, 0x00,
        0x0c, 0x0e, 0x00, 0x00, 0x63, 0xf3, 0x00, 0x00,
        0x01, 0xb2, 0x00, 0x00, 0x9f, 0x0a, 0x00, 0x00,
        0x30, 0x56, 0x00, 0x00, 0x37, 0xaa, 0x00, 0x00,
        0x3c, 0xa7, 0x00, 0x00, 0x93, 0xaf, 0x00, 0x00,
        0xad, 0x56, 0x00, 0x00, 0x0c, 0x4d, 0x00, 0x00,
        0x0d, 0x83, 0x00, 0x00, 0xca, 0x8e, 0x00, 0x00,
        0x7e, 0x23, 0x00, 0x00, 0xc0, 0xcf, 0x00, 0x00,
        0x7e, 0xa7, 0x00, 0x00, 0x72, 0x82, 0x00, 0x00,
        0x3a, 0xf5, 0x00, 0x00, 0x82, 0x52, 0x00, 0x00,
        0x2d, 0x43, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00,
};

TEST(HeapPageReadTest, GetId) {
    db::HeapPageId pid(-1, -1);
    db::Database::getCatalog().addTable(new db::SkeletonFile(-1, db::Utility::getTupleDesc(2)),
                                        db::Utility::generateUUID());

    db::HeapPage page(pid, EXAMPLE_DATA);
    EXPECT_EQ(pid, page.getId());
}

TEST(HeapPageReadTest, TestIterator) {
    db::HeapPageId pid(-1, -1);
    db::Database::getCatalog().addTable(new db::SkeletonFile(-1, db::Utility::getTupleDesc(2)),
                                        db::Utility::generateUUID());
    db::HeapPage page(pid, EXAMPLE_DATA);

    int row = 0;
    for (const db::Tuple &tup: page) {
        const db::Field &f0 = tup.getField(0);
        const db::Field &f1 = tup.getField(1);
        if (const auto *if0 = dynamic_cast<const db::IntField *>(&f0)) {
            EXPECT_EQ(EXAMPLE_VALUES[row][0], if0->getValue());
        } else {
            FAIL() << "Field 0 is not an IntField as expected";
        }
        if (const auto *if1 = dynamic_cast<const db::IntField *>(&f1)) {
            EXPECT_EQ(EXAMPLE_VALUES[row][1], if1->getValue());
        } else {
            FAIL() << "Field 0 is not an IntField as expected";
        }
        ++row;
    }
}

TEST(HeapPageReadTest, GetNumEmptySlots) {
    db::HeapPageId pid(-1, -1);
    db::Database::getCatalog().addTable(new db::SkeletonFile(-1, db::Utility::getTupleDesc(2)),
                                        db::Utility::generateUUID());
    db::HeapPage page(pid, EXAMPLE_DATA);
    EXPECT_EQ(484, page.getNumEmptySlots());
}

TEST(HeapPageReadTest, GetSlot) {
    db::HeapPageId pid(-1, -1);
    db::Database::getCatalog().addTable(new db::SkeletonFile(-1, db::Utility::getTupleDesc(2)),
                                        db::Utility::generateUUID());
    db::HeapPage page(pid, EXAMPLE_DATA);

    for (int i = 0; i < 20; ++i) {
        EXPECT_TRUE(page.isSlotUsed(i));
    }

    for (int i = 20; i < 504; ++i) {
        EXPECT_FALSE(page.isSlotUsed(i));
    }
}
