#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "config.h"
#include "logger.h"

#define CONF_PATH "/.local/share/file_sorter/config/config.conf"


static char *get_config_buff()
{
    // determine the absolute path of the config.
    char *username = getlogin();
    char *absolute = (char *) malloc((strlen(CONF_PATH) + strlen(username) + 7)
                                     * sizeof(char));

    if (absolute == NULL) {
        logger("Failed to allocate mamory.", ERROR);
        return NULL;
    }
    strcpy(absolute, "/home/");
    strcat(absolute, username);
    strcat(absolute, CONF_PATH);

    // read the size of the config.
    struct stat conf_stat;
    if (lstat(absolute, &conf_stat) != 0) return NULL; // TODO: Call the logger here.

    char *conf_buff = (char *) malloc((conf_stat.st_size + 1) * sizeof(char));
    if (conf_buff == NULL) return NULL;

    int conf_fd = open(absolute, O_RDONLY);
    if (conf_fd == -1) {
        logger("Failed to allocate mamory.", ERROR);
        return NULL;
    }

    if (read(conf_fd, conf_buff, conf_stat.st_size) == -1) return NULL; // TODO: Call the logger here.
    close(conf_fd);
    conf_buff[conf_stat.st_size] = '\0';

    free(absolute);
    return conf_buff;
}

static char *isolate_opt(const char *conf_buff, const char *opt)
{
    // don't change the original buffer.
    char *tmp = strdup(conf_buff);
    char *opt_c = strstr(tmp, opt);
    opt_c = strtok(opt_c, " "); // separete the option and the label.
    opt_c = strtok(NULL, "\n"); // get the option.
    opt_c = strdup(opt_c);

    free(tmp);
    return opt_c; // return value must be freed.
}

static inline unsigned int parse_int_opt(const char *conf_buff, const char *opt)
{
    char *opt_a = isolate_opt(conf_buff, opt);
    unsigned int opt_r = (opt_a == NULL) ? 0 : atoi(opt_a);
    
    free(opt_a);
    return opt_r; // get the int value.
}

static inline char *parse_str_opt(const char *conf_buff, const char *opt)
{
    // [!] TODO: Verify path before return.
    return isolate_opt(conf_buff, opt); // get the str value.
}

static char **parse_list(const char *conf_buff, const char *list)
{
    // locate the list.
    char *tmp = strdup(conf_buff);
    char *loc = strstr(tmp, list);
    if (loc == NULL) {
        logger("List is missing", WAR);
        return NULL;
    }

    char *curr_el = strtok(loc, "\n");
    unsigned int list_l = 10;
    unsigned int real_l = 0;
    char *(*list_r) = (char **) malloc(list_l * sizeof(char *));
    if (list_r == NULL) {
        logger("Failed to allocate mamory for list.", WAR);
        return NULL;
    }

    for (int c = 0; curr_el; c++) {
        curr_el = strtok(NULL, "\n");
        if (curr_el == NULL) break;
        if (!strcmp(curr_el, "[done]")) break;
        real_l = c + 1;

        if (real_l == list_l) {
            list_l += 10;
            list_r = (char **) realloc(list_r, list_l * sizeof(char *));
            if (list_r == NULL) {
                logger("Failed to reallocate mamory for list.", WAR);
                return NULL;
            }
        }
        list_r[c] = strdup(curr_el);
    }

    if (real_l == 0) {
        free(list_r);
        free(tmp);
        return NULL;
    }

    // null terminate the array.
    list_r[real_l] = NULL;
    free(tmp);
    return (char **) realloc(list_r, (real_l + 1) * sizeof(char *)); // adjust the size of the list.
}

static void update_list(const char *(*list),
                        const char *list_id,
                        FILE *conf)
{
    fprintf(conf, "%s\n", list_id);
    if (list == NULL) goto empt;

    for (int i = 0; list[i]; i++) {
        if (list[i] == NULL) continue; // the case where it is NULL is when we remove an element.
        fprintf(conf, "%s\n", list[i]);
    }
empt:fprintf(conf, "[done]\n\n");
}

static inline void free_list(char *(**list))
{
    if ((*list) == NULL) return;
    for (int i = 0; (*list)[i]; i++) {
        free((*list)[i]);
    }
    free(*list);
    *list = NULL;
}

void parse_config(struct config *dst)
{
    char *conf_buff = get_config_buff();
    if (!conf_buff) {
        logger("Failed to read config file.", ERROR);
        return;
    }

    dst->c_options.o_check_interval = parse_int_opt(conf_buff, "check_interval");
    dst->c_options.o_debug_log      = parse_int_opt(conf_buff, "debug_log") & 0x1;
    dst->c_options.o_default_path   = parse_str_opt(conf_buff, "default_dir_path");
    dst->c_options.o_enable_default = parse_int_opt(conf_buff, "enable_default_path") & 0x1;
    dst->c_options.o_move_no_ext    = parse_int_opt(conf_buff, "move_files_without_ext") & 0x1;
    dst->c_lists.l_check_list       = parse_list(conf_buff, "[check]");
    dst->c_lists.l_target_list      = parse_list(conf_buff, "[targets]");
    dst->c_lists.l_exclude_list     = parse_list(conf_buff, "[exclude]");
    free(conf_buff);
}

void destroy_config(struct config *src)
{
    // free the allocated space where it should.
    // free options.
    free(src->c_options.o_default_path);
    src->c_options.o_default_path = NULL;
    free_list(&src->c_lists.l_check_list);
    free_list(&src->c_lists.l_target_list);
    free_list(&src->c_lists.l_exclude_list);
}

void reparse_config(struct config *dst)
{
    destroy_config(dst);
    // reparse.
    parse_config(dst);
}

int update_config(const struct config *src)
{
    // determine the absolute path of the config.
    char *username = getlogin();
    char *absolute = (char *) malloc(sizeof(char) * (strlen(CONF_PATH) + 
                                                     strlen(username)  + 7));
    if (absolute == NULL) return -1;

    strcpy(absolute, "/home/");
    strcat(absolute, username);
    strcat(absolute, CONF_PATH);	

    // open the file.
    FILE *conf = fopen(absolute, "w");
    if (conf == NULL) return -1;

    // write updated options.
    fprintf(conf, "%s\n", "[basic_config]");
    fprintf(conf, "%s %d\n",   CHECK_INT,   src->c_options.o_check_interval);
    fprintf(conf, "%s %d\n",   DEBUG_LOG,   src->c_options.o_debug_log);
    fprintf(conf, "%s %s\n",   DEFAULT_DIR, src->c_options.o_default_path);
    fprintf(conf, "%s %d\n",   EN_DEFAULT,  src->c_options.o_enable_default);
    fprintf(conf, "%s %d\n\n", WITHOUT_EXT, src->c_options.o_move_no_ext);
    
    // write updated check list.
    update_list((const char **) src->c_lists.l_check_list,
                CHECK_LISTID,
                conf);

    update_list((const char **) src->c_lists.l_target_list,
                TARGET_LISTID,
                conf);

    update_list((const char **) src->c_lists.l_exclude_list,
                EXCLUDE_LISTID,
                conf);

    free(absolute);
    fclose(conf);

    return 0;
}
