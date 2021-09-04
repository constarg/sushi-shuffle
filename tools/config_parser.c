#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <config/config_parser.h>
#include <log/logger.h>
#include <mem.h>

#define CONFIG_REL_PATH             "/.local/share/file_sorter/config/config.conf"

#define CHECK_INTERVAL              "check_interval"
#define PARSE_INTERVAL              "parse_interval"
#define DEBUG                       "debug_log"
#define DEFAULT_DIR_PATH            "default_dir_path"
#define TARGETS                     "[targets]"
#define CHECK                       "[check]"
#define ENABLE_DEFAULT_PATH         "enable_default_path"

#define DONE_TARGETS                "[done_targets]"
#define DONE_CHECK                  "[done_check]"

#define SUCCESS_PARSE               "Successfully parse"
#define SUCCESS_LOAD                "Successfully load config"

#define PARSE_FAILED_EMPTY          "The value is empty"
#define PARSE_FAILED_NO_FIELD       "There is no such field"

#define FAILED_TO_OPEN              "Failed to open config file"
#define FAILED_TO_READ_SIZE         "Failed to read config size"
#define FAILED_TO_READ              "Failed to read config file"
#define FAILED_TO_CLOSE             "Failed to close config file"
#define FAILED_TO_PARSE             "Failed to parse"


static inline char *get_config_path() {
    char *username = getlogin();
    size_t config_path_s = strlen("/home/") + strlen(username) + strlen(CONFIG_REL_PATH);
    char *config_path = NULL;
    // Allocate memory using macro.
    ALLOCATE_MEM(config_path, config_path_s + 1, sizeof(char));

    // Form the path.
    strcpy(config_path, "/home/");
    strcat(config_path, username);
    strcat(config_path, CONFIG_REL_PATH);

    return config_path;
}

static inline char *form_parse_msg(const char *option, const char *type) {
    size_t parse_msg_s = strlen(type) + strlen(option);
    // TODO change the allocation.
    char *parse_msg = NULL;
    // Allocate memory.
    ALLOCATE_MEM(parse_msg, parse_msg_s + 2, sizeof(char));

    // form the error.
    strcpy(parse_msg, type);
    strcat(parse_msg, " ");
    strcat(parse_msg, option);

    return parse_msg;
}

static void *get_value_of(const char *option, const char *buffer) {
    // Make a copy of the buffer.
    char tmp_buffer[strlen(buffer) + 1];
    strcpy(tmp_buffer, buffer);
    // Find the location of the option inside the config.
    char *option_location = strstr(tmp_buffer, option);
    char *error_msg = form_parse_msg(option, FAILED_TO_PARSE);
    char *success_msg = form_parse_msg(option, SUCCESS_PARSE);

    // Check if the option exist.
    if (option_location == NULL) {
        make_log(error_msg, PARSE_FAILED_NO_FIELD, NORMAL_LOG, ERROR);
        return NULL;
    }

    // Split the option and the value.
    char *option_value = strtok(option_location, " ");
    if (option_value == NULL) {
        make_log(error_msg, PARSE_FAILED_EMPTY, NORMAL_LOG, ERROR);
        free(error_msg);
        free(success_msg);
        return NULL;
    }
    // Get the value.
    option_value = strtok(NULL, "\n");

    if (strcmp(option, DEFAULT_DIR_PATH) == 0) {
        make_log(success_msg, NULL, NORMAL_LOG, SUCCESS);
        free(error_msg);
        free(success_msg);
        // Result.
        char *result = NULL;
        ALLOCATE_MEM(result, strlen(option_value) + 1, sizeof(char));
        strcpy(result, option_value);
        return result;
    }

    int *int_value = NULL;
    // Allocate memory.
    ALLOCATE_MEM(int_value, 1, sizeof(int));
    *int_value = (int) strtol(option_value, &option_value, 10);

    make_log(success_msg, NULL, NORMAL_LOG, SUCCESS);
    free(error_msg);
    free(success_msg);
    return int_value;
}

static char **get_values_of(const char *list, const char *list_end, size_t *size, const char *buffer) {
    // Make a copy of the buffer content.
    char tmp_buffer[strlen(buffer) + 1];
    char *error_msg = form_parse_msg(list, FAILED_TO_PARSE);
    char *success_msg = form_parse_msg(list, SUCCESS_PARSE);
    strcpy(tmp_buffer, buffer);

    // Find the location in the config.
    char *list_location = strstr(buffer, list);
    if (list_location == NULL) {
        make_log(FAILED_TO_PARSE, error_msg, NORMAL_LOG, ERROR);
        free(error_msg);
        free(success_msg);
        return NULL;
    }

    size_t lines_s = 1;
    char **lines = NULL;
    // Allocate space for one line.
    ALLOCATE_MEM(lines, lines_s, sizeof(char *));
    // skip the first line.
    strtok(list_location, "\n");
    char *current_line = strtok(NULL, "\n");
    // Save the rest.
    while (strcmp(current_line, list_end) != 0) {
        // Allocate space for the current line.
        ALLOCATE_MEM(lines[lines_s - 1], strlen(current_line) + 1, sizeof(char ));
        // save the content
        strcpy(lines[lines_s - 1], current_line);
        // increase the size of the array to make it fit the next line.
        REALLOCATE_MEM(lines, sizeof(char *) * (++lines_s));
        current_line = strtok(NULL, "\n");
    }
    --lines_s;
    if (lines_s == 0) return NULL;

    *size = lines_s;

    make_log(success_msg, NULL, NORMAL_LOG, SUCCESS);
    free(error_msg);
    free(success_msg);
    return lines;
}

static void parse_data(struct config *conf, const char *buffer) {
    ALLOCATE_MEM(conf->c_checks_s, 1, sizeof(size_t));
    ALLOCATE_MEM(conf->c_targets, 1, sizeof(size_t));
    conf->c_check_interval = (int *) get_value_of(CHECK_INTERVAL, buffer);
    conf->c_parse_interval = (int *) get_value_of(PARSE_INTERVAL, buffer);
    conf->c_debug_log = (int *) get_value_of(DEBUG, buffer);
    conf->c_default_dir_path = (char *) get_value_of(DEFAULT_DIR_PATH, buffer);
    conf->c_enable_default_path = (int *) get_value_of(ENABLE_DEFAULT_PATH, buffer);
    conf->c_targets = get_values_of(TARGETS, DONE_TARGETS, conf->c_targets_s, buffer);
    conf->c_checks = get_values_of(CHECK, DONE_CHECK, conf->c_checks_s, buffer);
}

static inline void free_list(char **list, size_t size) {
    for (int line = 0; line < size; line++) free(list[line]);
    free(list);
}

struct config *clear_config(struct config *conf) {
    if (conf->c_check_interval != NULL) free(conf->c_check_interval);
    if (conf->c_parse_interval != NULL) free(conf->c_parse_interval);
    if (conf->c_debug_log != NULL) free(conf->c_debug_log);
    if (conf->c_default_dir_path != NULL) free(conf->c_default_dir_path);
    if (conf->c_targets != NULL) free_list(conf->c_targets, *conf->c_targets_s);
    if (conf->c_checks != NULL) free_list(conf->c_checks, *conf->c_checks_s);

    free(conf);
    struct config *tmp = NULL;
    ALLOCATE_MEM(tmp, 1, sizeof(struct config));
    return tmp;
}

int get_config(struct config *conf) {
    // Get the config path.
    char *config_path = get_config_path();

    // Try to open the config.
    int config_fd = open(config_path, O_RDONLY);
    if (config_fd == -1) return make_log(FAILED_TO_OPEN, strerror(errno), NORMAL_LOG, ERROR);

    // Get the size of the config file.
    struct stat file_stats;
    if (lstat(config_path, &file_stats) == -1) return make_log(FAILED_TO_READ_SIZE, strerror(errno), NORMAL_LOG, ERROR);

    // Read the config file.
    char *buffer = NULL;
    // Allocate space.
    ALLOCATE_MEM(buffer, file_stats.st_size + 1, sizeof(char));
    if (read(config_fd, buffer, file_stats.st_size) == -1)
        return make_log(FAILED_TO_READ, strerror(errno), NORMAL_LOG, ERROR);

    if (close(config_fd) == -1) return make_log(FAILED_TO_CLOSE, strerror(errno), NORMAL_LOG, ERROR);

    make_log(SUCCESS_LOAD, NULL, NORMAL_LOG, SUCCESS);

    parse_data(conf, buffer);
    free(config_path);
    free(buffer);
    return 0;
}
