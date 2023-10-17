include(FetchContent)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)

add_executable(pa2_test
        Bufferpool_test.cpp
        BTreeFile_test.cpp
)
target_link_libraries(pa2_test PRIVATE GTest::gtest_main db)

include(GoogleTest)
gtest_add_tests(TARGET pa2_test TEST_LIST tests)
foreach (test ${tests})
    set_tests_properties(${test} PROPERTIES TIMEOUT 5)
endforeach ()
