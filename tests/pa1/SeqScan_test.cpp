#include <gtest/gtest.h>
#include <stdexcept>
#include <db/Database.h>
#include <db/TupleDesc.h>
#include <db/DbFile.h>
#include <db/HeapFile.h>
#include <db/SeqScan.h>

TEST(SeqScanTest, IterateFile) {
    std::vector<db::Types::Type> types = {db::Types::INT_TYPE, db::Types::INT_TYPE};

    std::vector<std::string> names = {"field0", "field1"};

    db::TupleDesc descriptor(types, names);

    db::HeapFile table("heapfile.dat", descriptor);

    db::Database::getCatalog().addTable(&table, "test");

    db::SeqScan f(table.getId());

    EXPECT_NE(f.begin(), f.end());
    for (const auto& tup: f) {
        std::cout << tup.to_string() << std::endl;
    }
}
