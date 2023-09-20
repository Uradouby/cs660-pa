include(FetchContent)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(db_test
        Catalog_test.cpp
        HeapPageId_test.cpp
        HeapPageRead_test.cpp
        RecordId_test.cpp
        SeqScan_test.cpp
        TupleDesc_test.cpp
        Tuple_test.cpp
)
target_link_libraries(db_test PRIVATE GTest::gtest_main db)

include(GoogleTest)
gtest_discover_tests(db_test)
