cmake_minimum_required(VERSION 3.19)
project(File_Sorter_Core C)

set(CMAKE_C_STANDARD 99)

add_executable(file-sorter main.c tools/logger.c tools/include/log/logger.h tools/config_parser.c tools/include/config/config_parser.h src/manager.c src/include/manager.h tools/include/mem.h)

include_directories(
        src/include
        tools/include
)

set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/file_sorter/)

message("/home/$ENV{USER}/.local/share/file_sorter/ OK")
file(MAKE_DIRECTORY /home/$ENV{USER}/.local/share/file_sorter/config/)

message("/home/$ENV{USER}/.local/share/file_sorter/config/ OK")
file(MAKE_DIRECTORY /home/$ENV{USER}/default_sorter_path/)

message("/home/$ENV{USER}/default_sorter_path/ OK")
file(MAKE_DIRECTORY config/)

message("config/ OK")
file(MAKE_DIRECTORY service/)

message("service/ OK")
file(MAKE_DIRECTORY autostart/)
message("autostart/ OK")

if (EXISTS /home/$ENV{USER}/.config/systemd/)
    message("/home/$ENV{USER}/.config/systemd/ OK")
    if (EXISTS /home/$ENV{USER}/.config/systemd/user/)
        message("/home/$ENV{USER}/.config/systemd/user/ OK")
    else ()
        file(MAKE_DIRECTORY /home/$ENV{USER}/.config/systemd/user/)
        message("/home/$ENV{USER}/.config/systemd/user/ OK")
    endif ()
else ()
    file(MAKE_DIRECTORY /home/$ENV{USER}/.config/systemd/)
    message("/home/$ENV{USER}/.config/systemd/ OK")
    file(MAKE_DIRECTORY /home/$ENV{USER}/.config/systemd/user/)
    message("/home/$ENV{USER}/.config/systemd/user/ OK")
endif ()

file(WRITE config/config.conf
        [basic_config]\n
        check_interval\ 3000\n
        parse_interval\ 5000\n
        debug_log\ 0\n
        default_dir_path\ /home/$ENV{USER}/default_sorter_path/\n
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
