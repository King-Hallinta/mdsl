cmake_minimum_required(VERSION 3.10)

project(MDSL_Tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_combinators test_combinators.cpp)

target_include_directories(test_combinators PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

enable_testing()
add_test(NAME CombinatorTests COMMAND test_combinators)
