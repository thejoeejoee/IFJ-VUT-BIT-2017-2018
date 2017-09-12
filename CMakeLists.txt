cmake_minimum_required(VERSION 3.0)

project(ifj2017)

include(test/CMakeLists.txt)
enable_testing()
find_package(codecov)

add_executable(ifj2017_test test/interpreter.cpp test/llist.cpp)
target_link_libraries(ifj2017_test gtest)
add_test(NAME test COMMAND ifj2017_test)

add_executable(ifj2017 src/ifj2017.c src/interpreter.c src/interpreter.h src/llist.c src/llist.h)

add_coverage(ifj2017_test)
coverage_evaluate()