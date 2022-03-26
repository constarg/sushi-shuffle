cmake_minimum_required(VERSION 3.19)
project(File_Sorter_Core C)

set(CMAKE_C_STANDARD 17)

add_executable(file-sorter main.c src/config.c include/config.h)

include_directories(
       include/ 
)

set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
