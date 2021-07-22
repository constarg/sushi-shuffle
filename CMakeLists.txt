cmake_minimum_required(VERSION 3.19)
project(File_Sorter_Core C)

set(CMAKE_C_STANDARD 99)

add_executable(File_Sorter_Core main.c tools/logger.c tools/logger.h tools/configParser.c tools/configParser.h src/manager.c src/manager.h)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/file_sorter/)
file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/file_sorter/config/)
file(MAKE_DIRECTORY /home/$ENV{USER}/default_sorter_path/)
file(MAKE_DIRECTORY config/)

file(WRITE config/config.conf
        [basic_config]\n
        checkInterval\ 3000\n
        parseInterval\ 5000\n
        debugLog\ 0\n
        defaultDirPath\ /home/$ENV{USER}/default_sorter_path/\n
        \n
        [check]\n
        \n
        [done_check]\n
        \n
        [targets]\n
        \n
        [done_targets])

install(DIRECTORY config/ DESTINATION /home/$ENV{USER}/.local/share/file_sorter/config)