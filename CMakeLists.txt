cmake_minimum_required(VERSION 3.13)
project(ternary_search_tree)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(ternary_search_tree src/test.cc src/tst.h)