cmake_minimum_required(VERSION 3.19)
project(File_Sorter_Core C)

set(CMAKE_C_STANDARD 17)

set(SOURCE_FILES main.c ./src/config.c ./src/sorter.c ./src/logger.c)
set(HEADER_FILES ./include/config.h ./include/sorter.h ./include/logger.h)
set(TO_INCLUDE ./include/)

add_executable(file-sorter ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${TO_INCLUDE})

set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
