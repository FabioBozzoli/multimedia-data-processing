cmake_minimum_required(VERSION 3.31)
project(huff_string_table)

set(CMAKE_CXX_STANDARD 23)

add_executable(huff_string_table main.cpp table_generator.cpp hufstr.cpp)
