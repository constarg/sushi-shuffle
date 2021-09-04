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

#define SUCCESS_MESSAGE             "Successfully parse "
#define SUCCESS_LOAD                "Successfully load config"

#define ERROR_MESSAGE               "Failed to parse "
#define PARSE_FAILED_EMPTY          "The value is empty"
#define PARSE_FAILED_NOTHING        "There is no value"
#define PARSE_FAILED_NO_FIELD       "There is no such field"

#define FAILED_TO_OPEN              "Failed to open config file"
#define FAILED_TO_READ_SIZE         "Failed to read config size"
#define FAILED_TO_READ              "Failed to read config file"
#define FAILED_TO_CLOSE             "Failed to close config file"


static inline char *get_config_path() {
    char *username = getlogin();
    size_t config_path_s = strlen("/home/") + strlen(username) + strlen(CONFIG_REL_PATH);
    char *config_path = NULL;
    // Allocate memory using macro.
    ALLOCATE_MEM(config_path, config_path_s, sizeof(char));

    // Form the path.
    strcpy(config_path, "/home/");
    strcat(config_path, username);
    strcat(config_path, CONFIG_REL_PATH);

    return config_path;
}

static void *get_value_of(const char *option, const char *buffer) {


    return 0;
}

static char **get_values_of(const char *list, const char *buffer) {

    return NULL;
}

static void parse_data(struct config *conf, char *buffer) {
    conf->c_check_interval = (int *) get_value_of(CHECK_INTERVAL, buffer);
    conf->c_parse_interval = (int *) get_value_of(PARSE_INTERVAL, buffer);
    conf->c_debug_log = (int *) get_values_of(DEBUG, buffer);
    conf->c_targets = get_values_of(TARGETS, buffer);
    conf->c_checks = get_values_of(CHECK, buffer);
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
    if (read(config_fd, buffer, file_stats.st_size) == -1)
        return make_log(FAILED_TO_READ, strerror(errno), NORMAL_LOG, ERROR);

    if (close(config_fd) == -1) return make_log(FAILED_TO_CLOSE, strerror(errno), NORMAL_LOG, ERROR);

    make_log(SUCCESS_LOAD, NULL, NORMAL_LOG, SUCCESS);

    free(config_path);
    free(buffer);
    return 0;
}
