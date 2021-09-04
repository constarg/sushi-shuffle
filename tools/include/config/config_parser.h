#ifndef FILE_MANAGER_CONFIGPARSER_H
#define FILE_MANAGER_CONFIGPARSER_H 1

#include <stdlib.h>

struct config {
    int    *c_check_interval;
    int    *c_parse_interval;
    int    *c_debug_log;
    char   *c_default_dir_path;
    char   **c_targets;
    char   **c_checks;
    size_t *c_checks_s;
    size_t *c_targets_s;
};

extern int get_config(struct config *conf);

extern struct config *clear_config(struct config *conf);

#endif
