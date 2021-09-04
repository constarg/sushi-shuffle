#include <manager.h>

#include <config/config_parser.h>
#include <stdio.h>


int main() {
    struct config test;

    get_config(&test);

    printf("----- OPTIONS -----\n");
    printf("%d\n", *test.c_check_interval);
    printf("%d\n", *test.c_parse_interval);
    printf("%d\n", *test.c_debug_log);
    printf("%s\n", test.c_default_dir_path);
    /*printf("----- CHECKS -----\n");

    for (int i = 0; i < test.c_checks_s; i++) {
        printf("%s\n", test.c_checks[i]);
    }

    printf("----- TARGETS -----\n");

    for (int i = 0; i < test.c_targets_s; i++) {
        printf("%s\n", test.c_targets[i]);
    }*/
    //run();
    return 0;
}