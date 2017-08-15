cmake_minimum_required(VERSION 3.8)
project(untitled3)

set(CMAKE_C_STANDARD 90)

set(SOURCE_FILES Assembler.c Checkers.c Insertion.c)
add_executable(untitled3 ${SOURCE_FILES})

target_link_libraries(untitled3 m)