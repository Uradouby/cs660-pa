#include <gtest/gtest.h>
#include <stdexcept>
#include <db/RecordId.h>
#include <db/HeapPageId.h>

TEST(RecordIdTest, GetPageId) {
    db::HeapPageId hpid(-1, 2);
    db::RecordId hrid(&hpid, 3);
    EXPECT_EQ(&hpid, hrid.getPageId());
}

TEST(RecordIdTest, Tupleno) {
    db::HeapPageId hpid(-1, 2);
    db::RecordId hrid(&hpid, 3);
    EXPECT_EQ(3, hrid.getTupleno());
}

TEST(RecordIdTest, Equals) {
    db::HeapPageId hpid(-1, 2);
    db::HeapPageId hpid2(-1, 2);
    db::HeapPageId hpid3(-2, 2);

    db::RecordId hrid(&hpid, 3);
    db::RecordId hrid2(&hpid2, 3);
    db::RecordId hrid3(&hpid, 4);
    db::RecordId hrid4(&hpid3, 3);

    EXPECT_EQ(hrid, hrid2);
    EXPECT_EQ(hrid2, hrid);
    EXPECT_NE(hrid, hrid3);
    EXPECT_NE(hrid3, hrid);
    EXPECT_NE(hrid2, hrid4);
    EXPECT_NE(hrid4, hrid2);
}

TEST(RecordIdTest, HashCode) {
    db::HeapPageId hpid(-1, 2);
    db::RecordId hrid(&hpid, 3);
    db::HeapPageId hpid2(-1, 2);
    db::RecordId hrid2(&hpid2, 3);
    std::hash<db::RecordId> hasher;
    EXPECT_EQ(hasher(hrid), hasher(hrid2));
}
