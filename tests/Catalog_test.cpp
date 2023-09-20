#include <gtest/gtest.h>
#include <stdexcept>
#include <db/Database.h>
#include <db/Utility.h>
#include <db/TupleDesc.h>
#include <db/DbFile.h>
#include <db/SkeletonFile.h>

TEST(CatalogTest, GetTupleDesc) {
    int id1 = db::Utility::randomInt();
    std::string name1 = db::Utility::generateUUID();
    db::Database::getCatalog().clear();
    db::SkeletonFile file(id1, db::Utility::getTupleDesc(2));
    db::Database::getCatalog().addTable(&file, name1);

    db::TupleDesc expected = db::Utility::getTupleDesc(2);
    db::TupleDesc actual = db::Database::getCatalog().getTupleDesc(id1);

    EXPECT_EQ(expected, actual);
}

TEST(CatalogTest, GetTableId) {
    int id1 = db::Utility::randomInt();
    int id2 = id1 + 1;
    std::string name1 = db::Utility::generateUUID();
    std::string name2 = db::Utility::generateUUID();
    db::Database::getCatalog().clear();
    db::SkeletonFile file1(id1, db::Utility::getTupleDesc(2));
    db::SkeletonFile file2(id2, db::Utility::getTupleDesc(2));
    db::Database::getCatalog().addTable(&file1, name1);
    db::Database::getCatalog().addTable(&file2, name2);

    EXPECT_EQ(id1, db::Database::getCatalog().getTableId(name1));
    EXPECT_EQ(id2, db::Database::getCatalog().getTableId(name2));

    EXPECT_THROW(db::Database::getCatalog().getTableId("foo"), std::invalid_argument);
}

TEST(CatalogTest, GetDatabaseFile) {
    int id1 = db::Utility::randomInt();
    std::string name1 = db::Utility::generateUUID();
    db::Database::getCatalog().clear();
    db::SkeletonFile file(id1, db::Utility::getTupleDesc(2));
    db::Database::getCatalog().addTable(&file, name1);

    const db::DbFile *f = db::Database::getCatalog().getDatabaseFile(id1);
    EXPECT_EQ(id1, f->getId());
}

TEST(CatalogTest, HandleDuplicateNames) {
    int id1 = db::Utility::randomInt();
    int id2 = id1 + 1;
    std::string name1 = db::Utility::generateUUID();
    db::Database::getCatalog().clear();
    db::SkeletonFile file1(id1, db::Utility::getTupleDesc(2));
    db::Database::getCatalog().addTable(&file1, name1);

    db::SkeletonFile file2(id2, db::Utility::getTupleDesc(2));
    db::Database::getCatalog().addTable(&file2, name1);
    EXPECT_EQ(id2, db::Database::getCatalog().getTableId(name1));
}

TEST(CatalogTest, HandleDuplicateIds) {
    int id1 = db::Utility::randomInt();
    std::string name1 = db::Utility::generateUUID();
    std::string name2 = db::Utility::generateUUID();
    db::Database::getCatalog().clear();
    db::SkeletonFile file1(id1, db::Utility::getTupleDesc(2));
    db::Database::getCatalog().addTable(&file1, name1);

    db::SkeletonFile f(id1, db::Utility::getTupleDesc(2));
    db::Database::getCatalog().addTable(&f, name2);
    EXPECT_EQ(name2, db::Database::getCatalog().getTableName(id1));
    EXPECT_EQ(&f, db::Database::getCatalog().getDatabaseFile(id1));
}
