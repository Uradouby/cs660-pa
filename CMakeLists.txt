cmake_minimum_required(VERSION 3.22)
project(cs660 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(db)
#add_subdirectory(tests/pa1)
add_subdirectory(tests/pa2)
add_subdirectory(examples)

configure_file(heapfile.dat ${CMAKE_CURRENT_BINARY_DIR}/tests/pa1/heapfile.dat COPYONLY)
