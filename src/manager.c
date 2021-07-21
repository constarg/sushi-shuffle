#include <sys/types.h>
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include "manager.h"
#include "../tools/configParser.h"
#include "../tools/logger.h"

#define TRUE 1

#define PARSER_FAILED "Parser Failed"
#define PARSE_INTERVAL_NOT_FOUND "Parse interval not found, set default"
#define FAILED_TO_OPEN_DIR "Parser Failed to open a check dir."

#define DEFAULT_PARSE_INTERVAL 5000
#define RETRY_INTERVAL 3

_Noreturn void *parse(void *);
_Noreturn void *moveToDir(void *);
void checkMoveFile(char*);
struct config *conf;

void run() {
    conf = malloc(sizeof(struct config));
    pthread_t moveToDirThread;
    pthread_t parseThread;

    pthread_create(&parseThread, NULL, parse, NULL);
    pthread_create(&moveToDirThread, NULL, moveToDir, NULL);

    pthread_exit(NULL);
}

void *parse(void *arg) {

    while (TRUE) {
        conf = clearConfig(conf);
        if (getConfig(conf) == -1) makeLog(PARSER_FAILED, NULL, NORMAL_LOG, WARN);

        if (conf->parseInterval == NULL) {
            sleep(DEFAULT_PARSE_INTERVAL);
            if (*(conf->debugLog) == 1) makeLog(PARSE_INTERVAL_NOT_FOUND, NULL, DEBUG_LOG, WARN);
            continue;
        }

        sleep(*(conf->parseInterval));
    }
}


void *moveToDir(void *arg) {
    struct dirent *files;
    char* currDir;
    char* path;
    DIR* dir;

    while (TRUE) {
        if (conf->check == NULL) {
            sleep(RETRY_INTERVAL);
            continue;
        }
        //sprintf("%d ", *(conf->checkNumber));
        for (int currCheck = 1; strcmp(conf->check[currCheck], "[targets]") != 0; currCheck++) {
            currDir = conf->check[currCheck];
            dir = opendir(currDir);

            if (dir == NULL) makeLog(FAILED_TO_OPEN_DIR, NULL, DEBUG_LOG, ERROR);

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
        sleep(*(conf->checkInterval));
    }
}

void checkMoveFile(char* filepath) {
    char* currType;
}
