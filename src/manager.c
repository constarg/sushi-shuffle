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

#define TRUE                        1

#define PARSER_FAILED               "Parser Failed"
#define PARSE_INTERVAL_NOT_FOUND    "Parse interval not found, set default"
#define FAILED_TO_OPEN_DIR          "Parser Failed to open a check dir."
#define FAILED_TO_INITIALIZE_MUTEX  "Failed to initialize mutex."
#define FAILED_TO_CREATE_THREAD     "Failed to create thread."
#define FAILED_TO_MOVE_FILE         "Failed to move file."

#define SUCCESS_MOVE                "File moved successfully to target directory."

#define DEFAULT_PARSE_INTERVAL      5000
#define INITIAL_CHECK_WAIT          5


_Noreturn void *parse(void *arg);

_Noreturn void *move_to_dir(void *arg);

void check_move_file(char *filepath, char *file);

char *extract_file_extension_from_target(char *target, size_t *extension_len);

char *extract_path_from_target(char *target, char *filename);

struct config *conf;
pthread_mutex_t lock;

void run() {
    conf = calloc(1, sizeof(struct config));
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

_Noreturn void *parse(void *arg) {
    int interval;

    while (TRUE) {
        pthread_mutex_lock(&lock);
        conf = clear_config(conf);

        if (get_config(conf) == -1) make_log(PARSER_FAILED, NULL, NORMAL_LOG, WARN);

        if (conf->c_parse_interval == NULL) {
            sleep(DEFAULT_PARSE_INTERVAL);
            if (*(conf->c_debug_log) == 1) make_log(PARSE_INTERVAL_NOT_FOUND, NULL, DEBUG_LOG, WARN);
            continue;
        }
        interval = *(conf->c_parse_interval);
        pthread_mutex_unlock(&lock);

        sleep(interval);
    }
}


_Noreturn void *move_to_dir(void *arg) {
    sleep(INITIAL_CHECK_WAIT); // wait to make the first parse.
    struct dirent *files = NULL;
    char *curr_dir = NULL;
    char *path = NULL;
    int interval;
    size_t tmp_len;
    DIR *dir = NULL;

    while (TRUE) {
        pthread_mutex_lock(&lock);

        for (int curr_check = 0; curr_check < conf->c_check_number; curr_check++) {
            curr_dir = conf->c_check[curr_check];
            dir = opendir(curr_dir);

            if (dir == NULL) {
                if (*(conf->c_debug_log) == 1) make_log(FAILED_TO_OPEN_DIR, strerror(errno), DEBUG_LOG, ERROR);
                continue;
            }

            while ((files = readdir(dir)) != NULL) {

                if (files->d_type == DT_REG && files->d_name[0] != '.') {
                    tmp_len = strlen(curr_dir) + strlen(files->d_name) + 1;
                    path = calloc(tmp_len, sizeof(char));
                    strncpy(path, curr_dir, tmp_len);
                    strcat(path, files->d_name);
                    check_move_file(path, files->d_name);
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

void check_move_file(char *filepath, char *file) {
    char *curr_ext = NULL;
    char *check_ext = NULL;
    char *move_file_to = NULL;
    size_t tmp_len;


    if (conf->c_target_number == 0) {
        tmp_len = strlen(conf->c_default_dir_path) + strlen(file) + 1;
        move_file_to = calloc(tmp_len, sizeof(char));
        strncpy(move_file_to, conf->c_default_dir_path, tmp_len);
        strcat(move_file_to, file);

        if (rename(filepath, move_file_to) != 0)
            if (*(conf->c_debug_log) == 1) make_log(FAILED_TO_MOVE_FILE, strerror(errno), DEBUG_LOG, ERROR);

        free(move_file_to);
        make_log(SUCCESS_MOVE, NULL, NORMAL_LOG, SUCCESS);
        return;
    }

    for (int target = 0; target < conf->c_target_number; target++) {
        curr_ext = extract_file_extension_from_target(conf->c_targets_path[target], &tmp_len);

        check_ext = strstr(file, curr_ext);

        if (target+1 == conf->c_target_number && check_ext == NULL || check_ext[tmp_len] != '\0' && target + 1 == conf->c_target_number) {
            tmp_len = strlen(conf->c_default_dir_path) + strlen(file) + 1;
            move_file_to = calloc(tmp_len, sizeof(char));
            strncpy(move_file_to, conf->c_default_dir_path, tmp_len);
            strcat(move_file_to, file);

            if (rename(filepath, move_file_to) != 0) {
                if (*(conf->c_debug_log) == 1) make_log(FAILED_TO_MOVE_FILE, strerror(errno), DEBUG_LOG, ERROR);
                free(curr_ext);
                free(move_file_to);
                return;
            }

            free(curr_ext);
            free(move_file_to);
            make_log(SUCCESS_MOVE, NULL, NORMAL_LOG, SUCCESS);
            return;
        }
        else if (check_ext[tmp_len] != '\0') {
            free(curr_ext);
            continue;
        }
        move_file_to = extract_path_from_target(conf->c_targets_path[target], file);
        // insert the file into the path as destination name.
        strcat(move_file_to, file);

        if (rename(filepath, move_file_to) != 0) {
            if (*(conf->c_debug_log) == 1) make_log(FAILED_TO_MOVE_FILE, strerror(errno), DEBUG_LOG, ERROR);
            free(curr_ext);
            free(move_file_to);
            return;
        }
        free(curr_ext);
        free(move_file_to);
        make_log(SUCCESS_MOVE, NULL, NORMAL_LOG, SUCCESS);
        return;
    }
}

char *extract_file_extension_from_target(char *target, size_t *extension_len) {
    char *extension = NULL;
    char *tmp = NULL;
    char *splitter = " ";
    size_t target_len = strlen(target) + 1;

    tmp = calloc(target_len, sizeof(char));
    strncpy(tmp, target, target_len);
    strtok(tmp, splitter);

    target_len = strlen(tmp);
    *extension_len = target_len;
    extension = calloc(target_len + 1, sizeof(char));
    strncpy(extension, tmp, target_len);

    free(tmp);
    return extension;
}

char *extract_path_from_target(char *target, char *filename) {
    char *path = NULL;
    char *tmp = NULL;
    size_t target_len = strlen(target) + 1;

    tmp = calloc(target_len, sizeof(char));
    strncpy(tmp, target, target_len);

    target_len = strlen(strchr(tmp, '/')) + strlen(filename) + 1;
    path = calloc(target_len, sizeof(char));
    strncpy(path, strchr(tmp, '/'), target_len);

    free(tmp);
    return path;
}