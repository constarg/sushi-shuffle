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
#define INITIAL_CHECK_WAIT 5

_Noreturn void *parse(void *);

_Noreturn void *moveToDir(void *);

void checkMoveFile(char *, char *);

char *extractFileExtensionFromTarget(char *);

char *extractPathFromTarget(char *, char *);

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
    sleep(INITIAL_CHECK_WAIT); // wait to make the first parse.
    struct dirent *files = NULL;
    char *currDir = NULL;
    char *path = NULL;
    int interval;
    DIR *dir = NULL;

    while (TRUE) {
        pthread_mutex_lock(&lock);
        for (int currCheck = 1; strcmp(conf->check[currCheck], "[done_check]") != 0; currCheck++) {
            currDir = conf->check[currCheck];
            dir = opendir(currDir);

            if (dir == NULL) {
                if (*(conf->debugLog) == 1) makeLog(FAILED_TO_OPEN_DIR, strerror(errno), DEBUG_LOG, ERROR);
                continue;
            }

            while ((files = readdir(dir)) != NULL) {

                if (files->d_type == DT_REG && files->d_name[0] != '.') {
                    path = calloc((strlen(currDir) + strlen(files->d_name)), sizeof(char));
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
    char *currExt = NULL;
    char *checkExt = NULL;
    char *moveFileTo = NULL;
    int extLen;

    if (strcmp(conf->targetsPath[1], "[done_targets]") == 0) {
        moveFileTo = alloca(sizeof(char) * (strlen(filepath) + strlen(file)));
        strcpy(moveFileTo, conf->defaultDirPath);
        strcat(moveFileTo, file);
        if (rename(filepath, moveFileTo) != 0)
            if (*(conf->debugLog) == 1) makeLog(FAILED_TO_MOVE_FILE, strerror(errno), DEBUG_LOG, ERROR);

        makeLog(SUCCESS_MOVE, NULL, NORMAL_LOG, SUCCESS);
        return;
    }

    for (int target = 1; strcmp(conf->targetsPath[target], "[done_targets]") != 0; target++) {
        currExt = extractFileExtensionFromTarget(conf->targetsPath[target]);
        extLen = (int) strlen(currExt);

        checkExt = strstr(file, currExt);
        if (strcmp(conf->targetsPath[target + 1], "[done_targets]") == 0 && checkExt == NULL) {
            moveFileTo = calloc((strlen(filepath) + strlen(file)), sizeof(char));
            strcpy(moveFileTo, conf->defaultDirPath);
            strcat(moveFileTo, file);

            if (rename(filepath, moveFileTo) != 0) {
                if (*(conf->debugLog) == 1)  makeLog(FAILED_TO_MOVE_FILE, strerror(errno), DEBUG_LOG, ERROR);
                free(currExt);
                free(moveFileTo);
                return;
            }
            free(currExt);
            free(moveFileTo);
            makeLog(SUCCESS_MOVE, NULL, NORMAL_LOG, SUCCESS);
            return;
        }
        else if (checkExt == NULL || checkExt[extLen] != '\0') {
            free(currExt);
            continue;
        }

        moveFileTo = extractPathFromTarget(conf->targetsPath[target], file);
        // insert the file into the path as destination name.
        strcat(moveFileTo, file);

        if (rename(filepath, moveFileTo) != 0) {
            if (*(conf->debugLog) == 1) makeLog(FAILED_TO_MOVE_FILE, strerror(errno), DEBUG_LOG, ERROR);
            free(currExt);
            free(moveFileTo);
            return;
        }
        free(currExt);
        free(moveFileTo);
        makeLog(SUCCESS_MOVE, NULL, NORMAL_LOG, SUCCESS);
        return;
    }
}

char *extractFileExtensionFromTarget(char *target) {
    char *extension = NULL;
    char *tmp = NULL;
    char *splitter = " ";

    tmp = alloca(sizeof(char) * strlen(target));
    strcpy(tmp, target);
    strtok(tmp, splitter);

    // remove the redundant bytes.
    extension = malloc(sizeof(char) * strlen(tmp));
    strcpy(extension, tmp);

    return extension;
}

char *extractPathFromTarget(char *target, char *filename) {
    char *path = NULL;
    char *tmp = NULL;

    tmp = alloca(sizeof(char) * strlen(target));
    strcpy(tmp, target);
    path = calloc((strlen(strchr(tmp, '/') + strlen(filename))), sizeof(char));
    strcpy(path, strchr(tmp, '/'));

    return path;
}