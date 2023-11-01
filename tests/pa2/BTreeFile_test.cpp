#include <gtest/gtest.h>
#include <db/Database.h>
#include <db/BTreeFile.h>
#include <db/Utility.h>
#include <db/IntField.h>

TEST(BTreeFileTest, splitLeafPagesTest) {
    db::Database::reset();
    db::Catalog &catalog = db::Database::getCatalog();
    db::TupleDesc td = db::Utility::getTupleDesc(2);
    db::BTreeFile file("btree.dat", 0, td);
    catalog.addTable(&file);
    db::TransactionId tid;

    for(int i = 0; i < 600; i++)
    {
        db::Tuple tup(td);
        for (int j = 0; j < td.numFields(); j++) {
            tup.setField(j, new db::IntField(i));
        }
        file.insertTuple(tid, tup);
    }

    ASSERT_EQ(file.getNumPages(), 3);
}

TEST(BTreeFileTest, findLeafPagesTest) {
    db::Database::reset();
    db::BufferPool &bufferpool = db::Database::getBufferPool();
    db::Catalog &catalog = db::Database::getCatalog();
    db::TupleDesc td = db::Utility::getTupleDesc(2);
    db::BTreeFile file("btree1.dat", 0, td);
    catalog.addTable(&file);
    db::TransactionId tid;

    for(int i = 0; i < 1000; i++)
    {
        db::Tuple tup(td);
        for (int j = 0; j < td.numFields(); j++) {
            tup.setField(j, new db::IntField(i));
        }
        file.insertTuple(tid, tup);
    }

    db::PagesMap dirtypages{};
    db::BTreeRootPtrPage *rootPtr = file.getRootPtrPage(tid, dirtypages);
    db::BTreePageId *rootId = rootPtr->getRootId();
    db::IntField f(5);
    db::BTreeLeafPage *leaf_page = file.findLeafPage(tid, rootId, db::Permissions::READ_ONLY,&f);
    ASSERT_EQ(leaf_page->getId().pageNumber(), 1);
}