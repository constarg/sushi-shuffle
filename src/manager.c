#include <sys/types.h>
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <alloca.h>

#include <manager.h>
#include <config/config_parser.h>
#include <log/logger.h>
#include <mem.h>

#define TRUE                        1

#define PARSER_FAILED               "Parser Failed"
#define PARSE_INTERVAL_NOT_FOUND    "Parse interval not found, set default"
#define FAILED_TO_OPEN_DIR          "Parser Failed to open a check dir."
#define FAILED_TO_INITIALIZE_MUTEX  "Failed to initialize mutex."
#define FAILED_TO_CREATE_THREAD     "Failed to create thread."
#define FAILED_TO_MOVE_FILE         "Failed to move file."

#define SUCCESS_MOVE                "File moved successfully to target directory."

#define DEFAULT_PARSE_INTERVAL      5000
#define DEFAULT_ENABLE_PATH         1
#define INITIAL_CHECK_WAIT          5


static struct config *conf = NULL;
pthread_mutex_t lock;

_Noreturn void *parse(void *arg) {
    int interval;

    while(TRUE) {
        // Lock the mutex.
        pthread_mutex_lock(&lock);
        // Clear the config.
        conf = clear_config(conf);

        // Get the config.
        if (get_config(conf) == -1)
            make_log(PARSER_FAILED, NULL, NORMAL_LOG, WARN);

        // Check if interval parser exists.
        if (conf->c_parse_interval == NULL) {
            sleep(DEFAULT_PARSE_INTERVAL);
            if (*(conf->c_debug_log) == 1) make_log(PARSE_INTERVAL_NOT_FOUND, NULL, DEBUG_LOG, WARN);
        }
        // save the interval.
        interval = *(conf->c_parse_interval);
        pthread_mutex_unlock(&lock);
        // sleep.
        sleep(interval);
    }
}

static inline char *extract_path_from_target(const char *target) {
    char *path = NULL;
    char tmp[strlen(target) + 1];
    strcpy(tmp, target);

    size_t path_s = strlen(strchr(tmp, '/'));
    ALLOCATE_MEM(path, path_s + 1, sizeof(char));
    strcpy(path, strchr(tmp, '/'));

    return path;
}

static inline char *extract_extension_from_target(const char *target, size_t *ext_s) {
    char *extension = NULL;
    char tmp[strlen(target) + 1];

    // Make a copy of the target.
    strcpy(tmp, target);
    strtok(tmp, " ");

    // Allocate the space.
    size_t extension_s = strlen(tmp);
    ALLOCATE_MEM(extension, extension_s + 1, sizeof(char));
    // Copy the extension.
    strcpy(extension, tmp);

    *ext_s = extension_s;
    return extension;
}

static inline void move_to_default_path(const char *file_path, const char *file) {
    size_t move_file_to_s = strlen(conf->c_default_dir_path) + strlen(file);
    char *move_file_to = NULL;
    ALLOCATE_MEM(move_file_to, move_file_to_s + 1, sizeof(char));
    strcpy(move_file_to, conf->c_default_dir_path);
    strcat(move_file_to, file);

    if (rename(file_path, move_file_to) != 0)
        if (*(conf->c_debug_log) == 1) make_log(FAILED_TO_MOVE_FILE, strerror(errno), DEBUG_LOG, ERROR);

    free(move_file_to);
    make_log(SUCCESS_MOVE, NULL, NORMAL_LOG, SUCCESS);
}

static void move_file(const char *file_path, const char *file_name) {
    char *move_file_to = NULL;
    char *curr_ext = NULL;
    char *check_ext = NULL;
    char *curr_path = NULL;
    size_t move_file_to_s;
    size_t curr_ext_s;

    if (!conf->c_targets_s && conf->c_enable_default_path) {
        move_to_default_path(file_path, file_name);
        return;
    }

    if (!conf->c_targets_s) return;

    for (int target = 0; target < conf->c_targets_s; target++) {
        curr_ext = extract_extension_from_target(conf->c_targets[target], &curr_ext_s);
        check_ext = strstr(file_name, curr_ext);

        if (target + 1 == conf->c_targets_s && check_ext == NULL ||
            check_ext[curr_ext_s] != '\0' && target + 1 == conf->c_targets_s) {
            move_to_default_path(file_path, file_name);
            free(curr_ext);
        }
        else if (check_ext[curr_ext_s] != '\0') {
            free(curr_ext);
            continue;
        }
        curr_path = extract_path_from_target(conf->c_targets[target]);
        move_file_to_s = strlen(curr_path) + strlen(file_name);
        ALLOCATE_MEM(move_file_to, move_file_to_s + 1, sizeof(char));
        strcpy(move_file_to, curr_path);

        if (rename(file_path, move_file_to) != 0) {
            if (*(conf->c_debug_log) == 1) make_log(FAILED_TO_MOVE_FILE, strerror(errno), DEBUG_LOG, ERROR);
            free(curr_ext);
            free(curr_path);
            free(move_file_to);
            return;
        }

        free(curr_ext);
        free(move_file_to);
        free(curr_path);
        return;
    }


}

_Noreturn void *move_to_dir(void *arg) {
    sleep(INITIAL_CHECK_WAIT); // wait to make the first parse.
    char *curr_dir = NULL;
    char *path = NULL;
    size_t path_s;
    struct dirent *files = NULL;
    DIR *dir = NULL;
    int interval;

    while (TRUE) {
        pthread_mutex_lock(&lock);

        for (int curr_check = 0; curr_check < conf->c_checks_s; curr_check++) {
            curr_dir = conf->c_checks[curr_check];
            dir = opendir(curr_dir);

            if (dir == NULL) {
                if (*(conf->c_debug_log) == 1) make_log(FAILED_TO_OPEN_DIR, strerror(errno), DEBUG_LOG, ERROR);
                continue;
            }

            while ((files = readdir(dir)) != NULL) {
                if (files->d_type == DT_REG && files->d_name[0] != '.') {
                    path_s = strlen(curr_dir) + strlen(files->d_name);
                    ALLOCATE_MEM(path, path_s + 1, sizeof(char));
                    strcpy(path, curr_dir);
                    strcat(path, files->d_name);
                    move_file(path, files->d_name);
                    free(path);
                }
            }
            closedir(dir);
        }

        interval = *(conf->c_check_interval);
        pthread_mutex_unlock(&lock);
        sleep(interval);
    }
}


void run() {
    ALLOCATE_MEM(conf, 1, sizeof(struct config));

    pthread_t move_to_dir_thread;
    pthread_t parse_thread;

    if (pthread_mutex_init(&lock, NULL) != 0) make_log(FAILED_TO_INITIALIZE_MUTEX, strerror(errno), DEBUG_LOG, WARN);

    if (pthread_create(&parse_thread, NULL, parse, NULL) != 0)
        make_log(FAILED_TO_CREATE_THREAD, strerror(errno), DEBUG_LOG, WARN);
    if (pthread_create(&move_to_dir_thread, NULL, move_to_dir, NULL) != 0)
        make_log(FAILED_TO_CREATE_THREAD, strerror(errno), DEBUG_LOG, WARN);

    pthread_join(parse_thread, NULL);
    pthread_join(move_to_dir_thread, NULL);
    pthread_mutex_destroy(&lock);
}