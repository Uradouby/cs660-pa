#include <gtest/gtest.h>
#include <db/Database.h>
#include <db/BTreeFile.h>
#include <db/Utility.h>
#include <db/IntField.h>

TEST(BTreeFileTest, findLeafPage) {
    db::Database::reset();
    db::BufferPool &bufferpool = db::Database::getBufferPool();
    db::Catalog &catalog = db::Database::getCatalog();
    db::TupleDesc td = db::Utility::getTupleDesc(340);
    db::BTreeFile file("btree.dat", 0, td);
    catalog.addTable(&file);
    db::TransactionId tid;
    db::Tuple tup(file.getTupleDesc());

    for (int i = 0; i < td.numFields(); i++) {
        tup.setField(i, new db::IntField(i));
    }
    file.insertTuple(tid, tup);
    auto items = file.iterable(tid);
    int count = 0;
    for (auto it : items) {
        count++;
        std::cout << count << ' ' << it.to_string() << std::endl;
    }
}