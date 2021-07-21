#include <sys/types.h>
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <alloca.h>

#include "manager.h"
#include "../tools/configParser.h"
#include "../tools/logger.h"

#define TRUE 1

#define PARSER_FAILED "Parser Failed"
#define PARSE_INTERVAL_NOT_FOUND "Parse interval not found, set default"
#define FAILED_TO_OPEN_DIR "Parser Failed to open a check dir."
#define FAILED_TO_INITIALIZE_MUTEX "Failed to initialize mutex."
#define FAILED_TO_CREATE_THREAD "Failed to create thread."
#define FAILED_TO_MOVE_FILE "Failed to move file."

#define SUCCESS_MOVE "File moved successfully to target directory."

#define DEFAULT_PARSE_INTERVAL 5000
#define RETRY_INTERVAL 10

_Noreturn void *parse(void *);

_Noreturn void *moveToDir(void *);

void checkMoveFile(char *, char *);

struct config *conf;
pthread_mutex_t lock;

void run() {
    conf = malloc(sizeof(struct config));
    pthread_t moveToDirThread;
    pthread_t parseThread;

    if (pthread_mutex_init(&lock, NULL) != 0) makeLog(FAILED_TO_INITIALIZE_MUTEX, strerror(errno), DEBUG_LOG, WARN);

    if (pthread_create(&parseThread, NULL, parse, NULL) != 0)
        makeLog(FAILED_TO_CREATE_THREAD, strerror(errno), DEBUG_LOG, WARN);
    if (pthread_create(&moveToDirThread, NULL, moveToDir, NULL) != 0)
        makeLog(FAILED_TO_CREATE_THREAD, strerror(errno), DEBUG_LOG, WARN);

    pthread_join(parseThread, NULL);
    pthread_join(moveToDirThread, NULL);
    pthread_mutex_destroy(&lock);
}

void *parse(void *arg) {
    int interval;

    while (TRUE) {
        pthread_mutex_lock(&lock);
        conf = clearConfig(conf);
        if (getConfig(conf) == -1) makeLog(PARSER_FAILED, NULL, NORMAL_LOG, WARN);

        if (conf->parseInterval == NULL) {
            sleep(DEFAULT_PARSE_INTERVAL);
            if (*(conf->debugLog) == 1) makeLog(PARSE_INTERVAL_NOT_FOUND, NULL, DEBUG_LOG, WARN);
            continue;
        }
        interval = *(conf->parseInterval);
        pthread_mutex_unlock(&lock);
        sleep(interval);
    }
}


void *moveToDir(void *arg) {
    struct dirent *files;
    char *currDir;
    char *path;
    int interval;
    DIR *dir;

    while (TRUE) {
        pthread_mutex_lock(&lock);
        if (conf->check == NULL) {
            sleep(RETRY_INTERVAL);
            continue;
        }
        for (int currCheck = 1; strcmp(conf->check[currCheck], "[done_check]") != 0; currCheck++) {
            currDir = conf->check[currCheck];
            dir = opendir(currDir);

            if (dir == NULL) {
                if (*(conf->debugLog) == 1) makeLog(FAILED_TO_OPEN_DIR, strerror(errno), DEBUG_LOG, ERROR);
                continue;
            }

            while ((files = readdir(dir)) != NULL) {

                if (files->d_type == DT_REG && files->d_name[0] != '.') {
                    path = malloc(sizeof(char) * (strlen(currDir) + strlen(files->d_name)));
                    strcpy(path, currDir);
                    strcat(path, files->d_name);
                    checkMoveFile(path, files->d_name);

                    free(path);
                }
            }
            closedir(dir);
        }
        interval = *(conf->checkInterval);
        pthread_mutex_unlock(&lock);
        sleep(interval);
    }
}

void checkMoveFile(char *filepath, char *file) {
    char *currExt;
    char *splitter = " ";
    char *checkType;
    char *moveTo;

    for (int target = 1; strcmp(conf->targetsPath[target], "[done_targets]") != 0; target++) {
        currExt = alloca(sizeof(char) * strlen(conf->targetsPath[target]));
        strcpy(currExt, conf->targetsPath[target]);
        currExt = strtok(currExt, splitter);

        checkType = strstr(file, currExt);
        if (checkType == NULL) continue;

        moveTo = alloca(sizeof(char) * strlen(conf->targetsPath[target]));
        strcpy(moveTo, conf->targetsPath[target]);
        moveTo = strchr(moveTo, '/');
        strcat(moveTo, file);

        if (rename(filepath, moveTo) != 0) {
            if (*(conf->debugLog) == 1) makeLog(FAILED_TO_MOVE_FILE, strerror(errno), DEBUG_LOG, ERROR);
            return;
        }
        makeLog(SUCCESS_MOVE, NULL, NORMAL_LOG, SUCCESS);
        return;
    }
}
