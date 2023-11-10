#include <db/HeapFile.h>
#include <db/Database.h>
#include <db/SeqScan.h>
#include <db/Filter.h>
#include <db/IntField.h>
#include <db/JoinPredicate.h>
#include <db/Join.h>

using namespace db;

int main() {
    // construct a 3-column table schema

    std::vector<Types::Type> types = {Types::Type::INT_TYPE, Types::Type::INT_TYPE, Types::Type::INT_TYPE};

    std::vector<std::string> names = {"field0", "field1", "field2"};

    TupleDesc td(types, names);

    // create the tables, associate them with the data files
    // and tell the catalog about the schema  the tables.

    HeapFile table1("some_data_file1.dat", td);
    Database::getCatalog().addTable(&table1, "t1");

    HeapFile table2("some_data_file2.dat", td);
    Database::getCatalog().addTable(&table2, "t2");

    // construct the query: we use two SeqScans, which spoonfeed
    // tuples via iterators into join

    SeqScan ss1(table1.getId(), "t1");

    SeqScan ss2(table2.getId(), "t2");

    // create a filter for the where condition
    IntField operand(1);
    Predicate pred(0, Predicate::Op::GREATER_THAN, &operand);
    Filter sf1(pred, &ss1);

    int field1 = 1;
    int field2 = 1;
    JoinPredicate p(field1, Predicate::Op::EQUALS, field2);

    Join j(p, &sf1, &ss2);

    // and run it
    j.open();
    while (j.hasNext()) {
        auto tup = j.next();
        std::cout << tup.to_string() << std::endl;
    }
    j.close();
}