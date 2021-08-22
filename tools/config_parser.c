#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <config/config_parser.h>
#include <log/logger.h>

#define CONFIG_PATH "/.local/share/file_sorter/config/config.conf"

#define CHECK_INTERVAL "checkInterval"
#define PARSE_INTERVAL "parseInterval"
#define DEBUG "debugLog"
#define DEFAULT_DIR_PATH "defaultDirPath"
#define TARGETS "[targets]"
#define CHECK "[check]"

#define SUCCESS_MESSAGE "Successfully parse "
#define SUCCESS_LOAD "Successfully load config"

#define ERROR_MESSAGE "Failed to parse "
#define PARSE_FAILED_EMPTY "The value is empty"
#define PARSE_FAILED_NOTHING "There is no value"
#define PARSE_FAILED_NO_FIELD "There is no such field"

#define FAILED_TO_OPEN "Failed to open config file"
#define FAILED_TO_READ_SIZE "Failed to read config size"
#define FAILED_TO_READ "Failed to read config file"
#define FAILED_TO_CLOSE "Failed to close config file"


void parse_data(struct config *conf, const char *buffer, size_t buffer_size);

char **get_dependencies(const char *buffer, char *dependency, int* number);

int count_targets(char *location_of_targets, int len);

int get_dependency_length(const char* dependency);

int get_value_by_key_validator(const char *location_on_conf, const char *message, int index1, int index2);

void *get_value_by_key(const char* buffer, char* key, int is_integer, size_t buffer_size);

int get_config(struct config *conf) {
    char* username = getlogin();
    char* config_path = malloc(sizeof(char) * (strlen(CONFIG_PATH) + strlen(username) + strlen("home") + 3));
    strcpy(config_path, "/home/");
    strcat(config_path, username);
    strcat(config_path, CONFIG_PATH);

    int conf_fd = open(config_path, O_RDONLY);
    char *buffer = NULL;
    // error handling.
    if (conf_fd == -1) return make_log(FAILED_TO_OPEN, strerror(errno), NORMAL_LOG, ERROR);
    struct stat file_stats;

    if (lstat(config_path, &file_stats) == -1) return make_log(FAILED_TO_READ_SIZE, strerror(errno), NORMAL_LOG, ERROR);
    // make the size of the buffer equal to the config file size.
    buffer = malloc(sizeof(char) * file_stats.st_size);
    // error handling.
    if (read(conf_fd, buffer, file_stats.st_size) == -1)
        return make_log(FAILED_TO_READ, strerror(errno), NORMAL_LOG, ERROR);

    if (close(conf_fd)) return make_log(FAILED_TO_CLOSE, strerror(errno), NORMAL_LOG, ERROR);

    make_log(SUCCESS_LOAD, NULL, NORMAL_LOG, SUCCESS);
    parse_data(conf, buffer, file_stats.st_size);
    free(config_path);
    free(buffer);
    return 0;
}

void parse_data(struct config *conf, const char *buffer, size_t buffer_size) {
    conf->c_check_interval = (int *) get_value_by_key(buffer, CHECK_INTERVAL, 1, buffer_size);
    conf->c_parse_interval = (int *) get_value_by_key(buffer, PARSE_INTERVAL, 1, buffer_size);
    conf->c_debug_log = (int *) get_value_by_key(buffer, DEBUG, 1, buffer_size);
    conf->c_default_dir_path = (char *) get_value_by_key(buffer, DEFAULT_DIR_PATH, 0, buffer_size);
    conf->c_targets_path = get_dependencies(buffer, TARGETS, &conf->c_target_number);
    conf->c_check = get_dependencies(buffer, CHECK, &conf->c_check_number);
}

void *get_value_by_key(const char* buffer, char* key, int is_integer, size_t buffer_size) {
    char *tmp = NULL;
    tmp = malloc(sizeof(char) * (buffer_size));
    strncpy(tmp, buffer, buffer_size);

    char *location_on_conf = strstr(tmp, key);
    char error_message[50] = ERROR_MESSAGE;
    char success_message[50] = SUCCESS_MESSAGE;
    strcat(error_message, key);
    strcat(success_message, key);

    int key_len = (int) strlen(key);
    if (get_value_by_key_validator(location_on_conf, error_message, key_len, key_len + 1)) return NULL;

    strtok(location_on_conf, " ");
    char *value = strtok(NULL, "\n");

    if (!is_integer) {
        if (get_value_by_key_validator(location_on_conf, error_message, key_len, key_len + 1)) return NULL;
        size_t value_len = strlen(value) + 1;
        char *returned_value = malloc(sizeof(char) * value_len);
        strncpy(returned_value, value, value_len);
        make_log(success_message, NULL, NORMAL_LOG, SUCCESS);
        free(tmp);
        return returned_value;
    }

    if (get_value_by_key_validator(location_on_conf, error_message, key_len, key_len + 1)) return NULL;
    long tmp_integer = strtol(value, &value, 10);
    int *integer_value = malloc(sizeof(int));
    *integer_value = (int) tmp_integer;
    free(tmp);
    make_log(success_message, NULL, NORMAL_LOG, SUCCESS);

    return integer_value;
}

char **get_dependencies(const char *buffer, char *dependency, int* number) {
    int location_len = get_dependency_length(strstr(buffer, dependency));
    char *location_on_conf = calloc(location_len + 1, sizeof(char));
    strncpy(location_on_conf, strstr(buffer, dependency), location_len);

    char error_message[20] = ERROR_MESSAGE;
    char success_message[20] = SUCCESS_MESSAGE;
    strcat(error_message, dependency);
    strcat(success_message, dependency);

    if (location_len == 0) {
        make_log(error_message, PARSE_FAILED_EMPTY, NORMAL_LOG, ERROR);
        return NULL;
    }

    char *splitter = "\n";
    int dep_counter = count_targets(location_on_conf, location_len);

    char *current_dependency = strtok(location_on_conf, splitter);
    char **dependencies = malloc(sizeof(char *) * (dep_counter + 1));
    int save_counter = 0;
    size_t tmp_len = 0;
    // erase garbage.
    current_dependency = strtok(NULL, splitter);
    while (current_dependency != NULL && save_counter < dep_counter) {
        tmp_len = strlen(current_dependency);
        dependencies[save_counter] = calloc(tmp_len + 1, sizeof(char));
        strncpy(dependencies[save_counter], current_dependency, tmp_len);
        current_dependency = strtok(NULL, splitter);
        save_counter++;
    }
    *number = dep_counter;

    free(location_on_conf);
    make_log(success_message, NULL, NORMAL_LOG, SUCCESS);
    return dependencies;
}

int count_targets(char *location_of_targets, int len) {
    char* tmp_location = calloc(len + 1, sizeof(char));
    strncpy(tmp_location, location_of_targets, len);
    char* tmp = strtok(tmp_location, "\n");

    int counter = -1;
    while (tmp != NULL) {
        tmp = strtok(NULL, "\n");
        ++counter;
    }

    free(tmp_location);
    return counter;
}

int get_value_by_key_validator(const char *location_on_conf, const char *message, int index1, int index2) {
    if (location_on_conf == NULL) return make_log(message, PARSE_FAILED_NO_FIELD, NORMAL_LOG, ERROR);
    else if (location_on_conf[index1] == '\n') return make_log(message, PARSE_FAILED_NOTHING, NORMAL_LOG, ERROR);
    else if (location_on_conf[index2] == '\n') return make_log(message, PARSE_FAILED_EMPTY, NORMAL_LOG, ERROR);

    return 0;
}

int get_dependency_length(const char* dependency) {
    if (dependency == NULL) return 0;

    int len = 1;
    for (int ln = 1; dependency[ln] != '['; ln++) ++len;

    return len;
}

void freeDependencies(char** dependencies, int depNumber) {
    for (int curr_free = 0; curr_free < depNumber; curr_free++) {
        if (dependencies[curr_free] != NULL) free(dependencies[curr_free]);
    }
    free(dependencies);
}

struct config *clear_config(struct config *conf) {
    if (conf->c_check_interval != NULL) free(conf->c_check_interval);
    if (conf->c_parse_interval != NULL) free(conf->c_parse_interval);
    if (conf->c_debug_log != NULL) free(conf->c_debug_log);
    if (conf->c_default_dir_path != NULL) free(conf->c_default_dir_path);
    if (conf->c_targets_path != NULL) freeDependencies(conf->c_targets_path, conf->c_target_number);
    if (conf->c_check != NULL) freeDependencies(conf->c_check, conf->c_check_number);

    free(conf);
    return calloc(1, sizeof(struct config));
}


