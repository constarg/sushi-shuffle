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

file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/file_sorter/)


file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/file_sorter/config/)

file(MAKE_DIRECTORY /home/$ENV{USER}/default_sorter_path/)

file(MAKE_DIRECTORY config/)

file(MAKE_DIRECTORY service/)

file(MAKE_DIRECTORY autostart/)

if (EXISTS /home/$ENV{USER}/.config/systemd/)
    if (EXISTS /home/$ENV{USER}/.config/systemd/user/)
    else ()
        file(MAKE_DIRECTORY /home/$ENV{USER}/.config/systemd/user/)
    endif ()
else ()
    file(MAKE_DIRECTORY /home/$ENV{USER}/.config/systemd/)
    file(MAKE_DIRECTORY /home/$ENV{USER}/.config/systemd/user/)
endif ()

file(WRITE config/config.conf
        [basic_config]\n
        check_interval\ 3000\n
        parse_interval\ 5000\n
        debug_log\ 0\n
        default_dir_path\ /home/$ENV{USER}/default_sorter_path/\n
        enable_default_path\ 1\n
        \n
        [check]\n
        \n
        [done]\n
        \n
        [targets]\n
        \n
        [done]\n
	[exclude]\n
	[done])

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
        WantedBy=multi-user.target\n)

file(WRITE autostart/file-sorter.desktop
        [Desktop\ Entry]\n
        Version=1.0\n
        Type=Application\n
        Name=File\ Sorter\n
        Comment=File\ sorter\ helps\ you\ to\ organize\ your\ files.\n
        Exec=systemctl\ --user\ start\ file-sorter\n
        Terminal=false)

install(FILES config/config.conf PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ WORLD_WRITE WORLD_READ DESTINATION /home/$ENV{USER}/.local/share/file_sorter/config/)
install(FILES service/file-sorter.service PERMISSIONS OWNER_EXECUTE OWNER_READ WORLD_EXECUTE WORLD_READ DESTINATION /home/$ENV{USER}/.config/systemd/user/)
install(FILES autostart/file-sorter.desktop PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ GROUP_WRITE WORLD_EXECUTE WORLD_READ WORLD_WRITE DESTINATION /home/$ENV{USER}/.config/autostart/)
install(FILES build/file-sorter PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE DESTINATION /usr/bin/)
