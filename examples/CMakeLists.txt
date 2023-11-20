cmake_minimum_required(VERSION 3.22)
project(examples)

set(CMAKE_CXX_STANDARD 17)

add_executable(io io.cpp)
target_link_libraries(io PRIVATE db)

add_executable(iterator iterator.cpp)
add_executable(inheritance inheritance.cpp)
