#ifndef FILE_MANAGER_CONFIGPARSER_H
#define FILE_MANAGER_CONFIGPARSER_H 1

struct config {
    int *c_check_interval;
    int *c_parse_interval;
    int *c_debug_log;
    char *c_default_dir_path;
    char **c_targets_path;
    char **c_check;
    int c_check_number;
    int c_target_number;
};

extern int get_config(struct config *conf);

extern struct config *clear_config(struct config *conf);

#endif
