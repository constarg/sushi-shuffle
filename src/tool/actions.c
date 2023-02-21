#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "tool/actions.h"
#include "config.h"

/**
 * This functions chagnes a value of one of
 * the options in the config file.
 * @param n_value The new value to write.
 * @param wtchange The option to change.
 */
static int change_opt_value(const char *n_value,
                            const char *wtchange)
{
    struct config conf;
    init_config(&conf);
    parse_config(&conf);

    if (!strcmp(wtchange, CHECK_INT)) {
        conf.c_options.o_check_interval = atoi(n_value);
        if (conf.c_options.o_check_interval == 0) return -1;
    } else if (!strcmp(wtchange, DEBUG_LOG)) {
        conf.c_options.o_debug_log = atoi(n_value) & 0x1;
    } else if (!strcmp(wtchange, DEFAULT_DIR)) {
        free(conf.c_options.o_default_path);
        conf.c_options.o_default_path = (char *) strdup(n_value);
        // TODO - check if the path exist?
    } else if (!strcmp(wtchange, EN_DEFAULT)){
        conf.c_options.o_enable_default = atoi(n_value) & 0x1;
    } else {
        conf.c_options.o_move_no_ext = atoi(n_value) & 0x1;
    }

    if (update_config(&conf) == -1) return -1;
    destroy_config(&conf);
    return 0;
}

static inline size_t get_list_size(const char **list)
{
    if (list == NULL) return -1;

    size_t size;
    for (size = 0; list[size]; size++) {}

    return size;
}

static int add_to_list(const char *element, 
                       const char *list_id)
{
    char **tmp_list = NULL;
    size_t list_size = 0;
    struct config conf;
    init_config(&conf);
    parse_config(&conf);


    // decide what list to modify.
    if (!strcmp(list_id, CHECK_LISTID)) {
        tmp_list = conf.c_lists.l_check_list; 
    } else if (!strcmp(list_id, TARGET_LISTID)) {
        tmp_list = conf.c_lists.l_target_list;
    } else {
        tmp_list = conf.c_lists.l_exclude_list;
    }
    
    // get the size of the list.
    list_size = get_list_size((const char **) tmp_list);
    if (list_size == -1) list_size = 1;

    // add the new element.
    tmp_list = (char **) realloc(tmp_list, sizeof(char *) * (list_size + 2));
    tmp_list[list_size] = strdup(element);
    tmp_list[list_size + 1] = NULL; // terminate the array with NULL.

    conf.c_lists.l_check_list = tmp_list;
    
    if (update_config(&conf) == -1) return -1;
    destroy_config(&conf);
    return 0;
}

static int remove_from_list(int row, const char *list_id)
{
    char **tmp_list = NULL;
    size_t tmp_size = -1;
    struct config conf;
    init_config(&conf);
    parse_config(&conf);


    // decide what list to modify.
    if (!strcmp(list_id, CHECK_LISTID)) {
        tmp_list = conf.c_lists.l_check_list; 
    } else if (!strcmp(list_id, TARGET_LISTID)) {
        tmp_list = conf.c_lists.l_target_list;
    } else {
        tmp_list = conf.c_lists.l_exclude_list;
    }

    // Get the size of the list.
    tmp_size = get_list_size((const char **) tmp_list);
    if (tmp_size == -1 || row > tmp_size) return -1;

    // O(1) oparation to remove the element.
    free(tmp_list[row]);
    tmp_list[row] = NULL; // Make it NULL, so the updater ignore it.

    if (update_config(&conf) == -1) return -1;
    destroy_config(&conf);
    return 0;
}

int set_check_interval(const char *n_value)
{
    return change_opt_value(n_value, CHECK_INT);
}

int set_debug_log(const char *n_state)
{
    return change_opt_value(n_state, DEBUG_LOG);
}

int set_default_dir_path(const char *n_path)
{
    return change_opt_value(n_path, DEFAULT_DIR);
}

int set_enable_default_dir(const char *n_state)
{
    return change_opt_value(n_state, EN_DEFAULT);
}

int set_mv_without_ext(const char *n_state)
{
    return change_opt_value(n_state, WITHOUT_EXT);
}

int add_check(const char *path)
{
    return add_to_list(path, CHECK_LISTID);
}

int add_target(const char *ext, const char *path)
{
    int err_code = 0;
    char *build_target = (char *) malloc(sizeof(char) * 
                                        (strlen(ext)  + 
                                         strlen(path) + 2));
    sprintf(build_target, "%s %s", ext, path);
    err_code = add_to_list(build_target, TARGET_LISTID);
    free(build_target);
    return err_code;
}

int add_exclude(const char *ext, const char *path)
{
    int err_code = 0;
    char *build_target = (char *) malloc(sizeof(char)  * 
                                         (strlen(ext)  + 
                                          strlen(path) + 2));
    sprintf(build_target, "%s %s", ext, path);
    err_code = add_to_list(build_target, TARGET_LISTID);
    free(build_target);
    return err_code;
}

int remove_check(const char *row)
{
    return remove_from_list(atoi(row), CHECK_LISTID);
}

int remove_target(const char *row)
{
    return remove_from_list(atoi(row), TARGET_LISTID);
}

int remove_exclude(const char *row)
{
    return remove_from_list(atoi(row), EXCLUDE_LISTID);
}

int list_options()
{
    struct config conf;
    init_config(&conf);
    parse_config(&conf);
 
    printf("%s %d\n",   CHECK_INT,   conf.c_options.o_check_interval);
    printf("%s %d\n",   DEBUG_LOG,   conf.c_options.o_debug_log & 0x1);
    printf("%s %s\n",   DEFAULT_DIR, conf.c_options.o_default_path);
    printf("%s %d\n",   EN_DEFAULT,  conf.c_options.o_enable_default);
    printf("%s %d\n\n", WITHOUT_EXT, conf.c_options.o_move_no_ext);

    destroy_config(&conf);
    return 0;
}

int list_checks()
{
    struct config conf;
    init_config(&conf);
    parse_config(&conf);

    if (conf.c_lists.l_check_list == NULL) return -1;
    for (int i = 0; 
         conf.c_lists.l_check_list[i]; i++) {
        printf("[%d] %s\n", i, conf.c_lists.l_check_list[i]);
    }

    destroy_config(&conf);
    return 0;
}
  
int list_targets()
{
    struct config conf;
    init_config(&conf);
    parse_config(&conf);

    if (conf.c_lists.l_target_list == NULL) return -1;
    for (int i = 0; 
         conf.c_lists.l_target_list[i]; i++) {
        printf("[%d] %s\n", i, conf.c_lists.l_target_list[i]);
    }

    destroy_config(&conf);
    return 0;
}

int list_excludes()
{
    struct config conf;
    init_config(&conf);
    parse_config(&conf);

    if (conf.c_lists.l_exclude_list == NULL) return -1;
    for (int i = 0; 
         conf.c_lists.l_exclude_list[i]; i++) {
        printf("[%d] %s\n", i, conf.c_lists.l_exclude_list[i]);
    }

    destroy_config(&conf);
    return 0;
}
