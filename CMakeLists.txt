cmake_minimum_required(VERSION 3.19)
project(File_Sorter_Core C)

set(CMAKE_C_STANDARD 99)

add_executable(file-sorter main.c tools/logger.c tools/logger.h tools/configParser.c tools/configParser.h src/manager.c src/manager.h)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/file_sorter/)
file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/file_sorter/config/)
file(MAKE_DIRECTORY /home/$ENV{USER}/default_sorter_path/)
file(MAKE_DIRECTORY config/)
file(MAKE_DIRECTORY service)

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

file(WRITE service/file-sorter.service
        [Unit]\n
        Description=File\ sorter\ helps\ you\ to\ organize\ your\ files.\n
        \n
        [Service]\n
        Type=simple\n
        ExecStart=/bin/bash\ -c\ /usr/bin/file-sorter\n
        Restart=on-failure\n
        Group=$ENV{USER}\n
        \n
        [Install]\n
        WantedBy=multi-user.target\n
)

install(FILES config/config.conf PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ WORLD_EXECUTE WORLD_READ DESTINATION /home/$ENV{USER}/.local/share/file_sorter/config/)
install(FILES service/file-sorter.service PERMISSIONS OWNER_EXECUTE OWNER_READ WORLD_EXECUTE WORLD_READ DESTINATION /home/$ENV{USER}/.config/systemd/user/)
install(FILES build/file-sorter PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE DESTINATION /usr/bin/)