cmake_minimum_required(VERSION 3.19)
project(File_Sorter_Core C)

set(CMAKE_C_STANDARD 99)

add_executable(File_Sorter_Core main.c tools/logger.c tools/logger.h tools/configParser.c tools/configParser.h src/manager.c src/manager.h)