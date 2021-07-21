#include <sys/types.h>
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "manager.h"
#include "../tools/configParser.h"
#include "../tools/logger.h"

#define TRUE 1

#define PARSER_FAILED "Parser Failed"
#define PARSE_INTERVAL_NOT_FOUND "Parse interval not found, set default"
#define FAILED_TO_OPEN_DIR "Parser Failed to open a check dir."
#define FAILED_TO_INITIALIZE_MUTEX "Failed to initialize mutex."
#define FAILED_TO_CREATE_THREAD "Failed to create thread."

#define DEFAULT_PARSE_INTERVAL 5000
#define RETRY_INTERVAL 3

_Noreturn void *parse(void *);
_Noreturn void *moveToDir(void *);
void checkMoveFile(char*);

struct config *conf;
pthread_mutex_t lock;

void run() {
    conf = malloc(sizeof(struct config));
    pthread_t moveToDirThread;
    pthread_t parseThread;

    if (pthread_mutex_init(&lock, NULL) != 0) makeLog(FAILED_TO_INITIALIZE_MUTEX, strerror(errno), DEBUG_LOG, WARN);

    if (pthread_create(&parseThread, NULL, parse, NULL) != 0) makeLog(FAILED_TO_CREATE_THREAD, strerror(errno), DEBUG_LOG, WARN);
    if (pthread_create(&moveToDirThread, NULL, parse, NULL) != 0) makeLog(FAILED_TO_CREATE_THREAD, strerror(errno), DEBUG_LOG, WARN);

    pthread_join(parseThread, NULL);
    pthread_join(moveToDirThread, NULL);
    pthread_mutex_destroy(&lock);
}

void *parse(void *arg) {

    while (TRUE) {
        pthread_mutex_lock(&lock);
        conf = clearConfig(conf);
        if (getConfig(conf) == -1) makeLog(PARSER_FAILED, NULL, NORMAL_LOG, WARN);

        if (conf->parseInterval == NULL) {
            sleep(DEFAULT_PARSE_INTERVAL);
            if (*(conf->debugLog) == 1) makeLog(PARSE_INTERVAL_NOT_FOUND, NULL, DEBUG_LOG, WARN);
            continue;
        }

        pthread_mutex_unlock(&lock);
        sleep(*(conf->parseInterval));
    }
}


void *moveToDir(void *arg) {
    struct dirent *files;
    char* currDir;
    char* path;
    DIR* dir;

    while (TRUE) {
        pthread_mutex_lock(&lock);
        if (conf->check == NULL) {
            sleep(RETRY_INTERVAL);
            continue;
        }
        for (int currCheck = 1; strcmp(conf->check[currCheck], "[targets]") != 0; currCheck++) {
            currDir = conf->check[currCheck];
            dir = opendir(currDir);

            if (dir == NULL) {
                makeLog(FAILED_TO_OPEN_DIR, strerror(errno), DEBUG_LOG, ERROR);
                continue;
            }

            while ((files = readdir(dir)) != NULL) {

                if (files->d_type == DT_REG) {
                    path = malloc(sizeof(char) * (strlen(currDir) + strlen(files->d_name)));
                    strcpy(path, currDir);
                    strcat(path, files->d_name);
                    // TODO move the to the desire dir.
                    free(path);
                }
            }
            closedir(dir);
        }
        pthread_mutex_unlock(&lock);
        sleep(*(conf->checkInterval));
    }
}

void checkMoveFile(char* filepath) {
    char* currType;
}
