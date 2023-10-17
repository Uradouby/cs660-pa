#include <gtest/gtest.h>
#include <stdexcept>
#include <db/HeapPageId.h>

TEST(HeapPageIdTest, GetTableId) {
    db::HeapPageId pid(1, 1);
    EXPECT_EQ(1, pid.getTableId());
}

TEST(HeapPageIdTest, PageNumber) {
    db::HeapPageId pid(1, 1);
    EXPECT_EQ(1, pid.pageNumber());
}

TEST(HeapPageIdTest, HashCode) {
    const std::hash<db::PageId> hasher;

    db::HeapPageId pid1(1, 1);
    std::size_t code1 = hasher(pid1);
    EXPECT_EQ(code1, hasher(pid1));
    EXPECT_EQ(code1, hasher(pid1));

    db::HeapPageId pid2(2, 2);
    std::size_t code2 = hasher(pid2);
    EXPECT_EQ(code2, hasher(pid2));
    EXPECT_EQ(code2, hasher(pid2));
}

TEST(HeapPageIdTest, Equals) {
    db::HeapPageId pid1 = db::HeapPageId(1, 1);
    db::HeapPageId pid1Copy = db::HeapPageId(1, 1);
    db::HeapPageId pid2 = db::HeapPageId(2, 2);

    EXPECT_EQ(pid1, pid1);
    EXPECT_EQ(pid1, pid1Copy);
    EXPECT_EQ(pid1Copy, pid1);
    EXPECT_EQ(pid2, pid2);

    EXPECT_NE(pid1, pid2);
    EXPECT_NE(pid1Copy, pid2);
    EXPECT_NE(pid2, pid1);
    EXPECT_NE(pid2, pid1Copy);
}
