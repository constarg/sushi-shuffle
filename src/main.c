#ifdef __unix__

#include <malloc.h>

#include "config.h"
#include "sorter.h"
#include "tool/actions.h"

#else
#include <stdio.h>
#endif

const char usage[] = "Usage:\n \tsushi-shuffle [OPTION] ...\n\n"
                     "\t--start-sorter.           Start the sorter service\n"
                     "\t--set-check-interval      [value] Change the value of check interval.\n"
                     "\t--set-default-dir-path    [path] Change the default directory path.\n"
                     "\t--set-enable-default-dir  [value] 0:1 Enable the to transfer files in default dir.\n"
                     "\t--set-debug-log           [value] 0:1 Change the log to debug mode (1).\n"
                     "\t--set-mv-without-ext      [value] 0:1 Enable the program to move files without extention.\n"
                     "\t--add-check               [path] Add new check.\n"
                     "\t--add-target              [ext] [path] Add new target.\n"
                     "\t--remove-check            [row number] remove check.\n"
                     "\t--remove-target           [row number] remove target.\n"
                     "\t--list-checks list checks.\n"
                     "\t--list-targets list targets.\n"
                     "\t--list-options list options.\n\n\n"
                     "Each of the above commands will print an OK message on success.\n"
                     "Check https://github.com/constarg/sushi-shuffle for more information's about the project!.\n";

static inline int help()
{
    return printf("%s\n", usage);
}


int main(int argc, char *argv[])
{
#ifdef __unix__

    if (argv[1] == NULL) {
        return help();
    } else if (!strcmp(argv[1], "--start-sorter")) {
        struct config config_p;
        // initialzize the config.
        init_config(&config_p);
        // parse config.
        parse_config(&config_p);
        // start procces
        start_sorter(&config_p);
        destroy_config(&config_p);
    } else if (!strcmp(argv[1], "--set-check-interval")) {
        if (argv[2] == NULL) return help();
        if (set_check_interval(argv[2]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--set-default-dir-path")) {
        if (argv[2] == NULL) return help();
        set_default_dir_path(argv[2]);
    } else if (!strcmp(argv[1], "--set-enable-default-dir")) {
        if (argv[2] == NULL) return help();
        if (set_enable_default_dir(argv[2]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--set-debug-log")) {
        if (argv[2] == NULL) return help();
        if (set_debug_log(argv[2]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--set-mv-without-ext")) {
        if (argv[2] == NULL) return help();
        if (set_mv_without_ext(argv[2]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--add-check")) {
        if (argv[2] == NULL) return help();
        if (add_check(argv[2]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--add-target")) {
        if (argv[2] == NULL) return help();
        if (argv[3] == NULL) return help();
        if (add_target(argv[2], argv[3]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--add-exclude")) {
        if (argv[2] == NULL) return help();
        if (argv[3] == NULL) return help();
        if (add_exclude(argv[2], argv[3]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--remove-check")) {
        if (argv[2] == NULL) return help();
        if (remove_check(argv[2]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--remove-target")) {
        if (argv[2] == NULL) return help();
        if (remove_target(argv[2]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--remove-exclude")) {
        if (argv[2] == NULL) return help();
        if (remove_exclude(argv[2]) == -1) {
            printf("Error\n");
        } else {
            printf("OK\n");
        }
    } else if (!strcmp(argv[1], "--list-options")) {
        list_options();
    } else if (!strcmp(argv[1], "--list-checks")) {
        list_checks();
    } else if (!strcmp(argv[1], "--list-targets")) {
        list_targets();
    } else if (!strcmp(argv[1], "--list-excludes")) {
        list_excludes();
    } else {
        return help();
    }

#else
    printf("The oparation system does not support this binary.\n");
#endif
}
