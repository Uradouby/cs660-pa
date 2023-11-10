# Programming Assignment 3

In this assignment, you will write a set of operators to implement table modifications (e.g., insert and delete
records), selections, joins, and aggregates. These will build on top of the foundation that you wrote in PA 1 to
provide you with a database system that can perform simple queries over multiple tables.

Additionally, you will also utilize the buffer eviction code you have developed in the previous assignment.

## Filter and Join

`DbIterator` classes implement the operations of the relational algebra. You will now implement two operators that will
enable you to perform queries that are slightly more interesting than a table scan.

- `Filter`: This operator only returns tuples that satisfy a `Predicate` that is specified as part of its constructor.
  Hence, it filters out any tuples that do not match the predicate.

- `Join`: This operator joins tuples from its two children according to a `JoinPredicate` that is passed in as part of
  its constructor. We require a simple nested loop join implementation and a hash join implementation respectively, but
  you may explore more interesting join implementations. Describe your implementation in your writeup.

## Aggregate

Aggregate operators implement basic SQL aggregates with a GROUP BY clause. You should implement the five SQL aggregates
(`COUNT`, `SUM`, `AVG`, `MIN`, `MAX`) and support grouping. You only need to support aggregates over a single field, and
grouping by a single field.

In order to calculate aggregates, we use an `Aggregator` interface which merges a new tuple into the existing
calculation of an aggregate. The `Aggregator` is told during construction what operation it should use for
aggregation. Subsequently, the client code should call `Aggregator::mergeTupleIntoGroup` for every tuple in the child
iterator. After all tuples have been merged, the client can retrieve a DbIterator of aggregation results. Each tuple in
the result is a pair of the form (`groupValue`, `aggregateValue`), unless the value of the group by field
was `Aggregator::NO_GROUPING`, in which case the result is a single tuple of the form (`aggregateValue`).

Note that this implementation requires space linear in the number of distinct groups. For the purposes of this
assignment, you do not need to worry about the situation where the number of groups exceeds available memory.

## HeapFile Mutability

Now, we will begin to implement methods to support modifying tables. We begin at the level of individual pages and
files. There are two main sets of operations: adding tuples and removing tuples.

- Removing tuples: To remove a tuple, you will need to implement `deleteTuple`. Tuples contain RecordIDs which allow you
  to find the page they reside on, so this should be as simple as locating the page a tuple belongs to and modifying the
  headers of the page appropriately.

- Adding tuples: The `insertTuple` method in `HeapFile` is responsible for adding a tuple to a heap file. To add a new
  tuple to a `HeapFile`, you will have to find a page with an empty slot. If no such pages exist in the `HeapFile`, you
  need to create a new page and append it to the physical file on disk. You will need to ensure that the `RecordID`
  in
  the tuple is updated correctly.

To implement `HeapPage`, you will need to modify the header bitmap in methods such as `insertTuple` and `deleteTuple`.
You may find that the `getNumEmptySlots` and `isSlotUsed` methods we asked you to implement in PA 1 serve as useful
abstractions. Note that there is a `markSlotUsed` method provided as an abstraction to modify the filled or cleared
status of a tuple in the page header.

## Insertion and deletion

Now that you have can add and remove tuples in `HeapFile`s, you will implement the `Insert` and `Delete` operators.

For plans that implement insert and delete queries, the top-most operator is a special `Insert` or `Delete` operator
that modifies the pages on disk. These operators return the number of affected tuples. This is implemented by returning
a single tuple with one integer field, containing the count.

- `Insert`: This operator adds the tuples it reads from its child operator to the `tableid` specified in its
  constructor. It should use the `BufferPool::insertTuple` method to do this.
- `Delete`: This operator deletes the tuples it reads from its child operator from the `tableid` specified in its
  constructor. It should use the `BufferPool::deleteTuple` method to do this.

## Exercises

Implement the methods as indicated by the `// TODO pa3.x` comments in the following files:

### Exercise 1: Filter and Join

- Filter ([header](../include/db/Filter.h) & [implementation](../db/Filter.cpp))
- Join ([header](../include/db/Join.h) & [implementation](../db/Join.cpp))
- JoinPredicate ([header](../include/db/JoinPredicate.h) & [implementation](../db/JoinPredicate.cpp))
- HashEquiJoin ([header](../include/db/HashEquiJoin.h) & [implementation](../db/HashEquiJoin.cpp))

### Exercise 2: Aggregate

- Aggregate ([header](../include/db/Aggregate.h) & [implementation](../db/Aggregate.cpp))
- IntegerAggregator ([header](../include/db/IntegerAggregator.h) & [implementation](../db/IntegerAggregator.cpp))
- StringAggregator ([header](../include/db/StringAggregator.h) & [implementation](../db/StringAggregator.cpp))

### Exercise 3: HeapFile Mutability

- HeapFile ([header](../include/db/HeapFile.h) & [implementation](../db/HeapFile.cpp))
- HeapPage ([header](../include/db/HeapPage.h) & [implementation](../db/HeapPage.cpp))

### Exercise 4: Insertion and deletion

- Insert ([header](../include/db/Insert.h) & [implementation](../db/Insert.cpp))
- Delete ([header](../include/db/Delete.h) & [implementation](../db/Delete.cpp))

