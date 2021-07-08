#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "configParser.h"
#include "logger.h"

#define CONFIG_PATH "../config/config.conf"

#define TIMEOUT "timeout"
#define SORT_ON_REBOOT "sortOnReboot"
#define DEBUG "debugLog"
#define DEFAULT_DIR_PATH "default_dir_path"
#define TARGETS "[targets]"

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


void parseData(struct config*, char*);
char** getTargets(char*);
int countTargets(const char*);
int hasParseError(const char*, const char*, int, int);
void* getValueByKey(const char*, char*,int);

int getConfig(struct config* conf) {
    // get the file descriptor of config file.
    int confFD = open(CONFIG_PATH, O_RDONLY);
    char *buffer = NULL;

    // error handling.
    if (confFD == -1) return makeLog(FAILED_TO_OPEN, strerror(errno), NORMAL_LOG, ERROR);

    struct stat fileStats;

    if (lstat(CONFIG_PATH, &fileStats) == -1) return makeLog(FAILED_TO_READ_SIZE, strerror(errno), NORMAL_LOG, ERROR);
    // make the size of the buffer equal to the config file size.
    buffer = malloc(sizeof(char) * fileStats.st_size);
    // error handling.
    if (read(confFD, buffer, fileStats.st_size) == -1) return makeLog(FAILED_TO_READ, strerror(errno), NORMAL_LOG, ERROR);
    if (close(confFD)) return makeLog(FAILED_TO_CLOSE, strerror(errno), NORMAL_LOG, ERROR);

    makeLog(SUCCESS_LOAD, NULL, NORMAL_LOG, SUCCESS);
    parseData(conf, buffer);
    free(buffer);
    return 1;
}

void parseData(struct config* conf, char* buffer) {
    conf -> timeout = (int*) getValueByKey(buffer, TIMEOUT, 1);
    conf -> sortOnReboot = (int*) getValueByKey(buffer, SORT_ON_REBOOT, 1);
    conf -> debug = (int*) getValueByKey(buffer, DEBUG, 1);
    conf -> defaultFolderPath = (char*) getValueByKey(buffer, DEFAULT_DIR_PATH, 0);
    conf -> targetsPath = getTargets(buffer);
}

void* getValueByKey(const char* buffer, char key[], int isInteger) {
    char* tmp = malloc(sizeof(char) * strlen(buffer));
    strcpy(tmp, buffer);
    char* locationOnConf = strstr(tmp, key);
    char errorMessage[50] = ERROR_MESSAGE;
    char successMessage[50] = SUCCESS_MESSAGE;
    strcat(errorMessage, key);
    strcat(successMessage, key);
    int keyLen = (int) strlen(key);

    if (hasParseError(locationOnConf, errorMessage, keyLen, keyLen+1)) return NULL;

    strtok(locationOnConf, " ");
    char* value = strtok(NULL, "\n");

    if (!isInteger) {
        if (hasParseError(locationOnConf, errorMessage, keyLen, keyLen+1)) return NULL;
        makeLog(successMessage, NULL, NORMAL_LOG, SUCCESS);
        return value;
    }
    if (hasParseError(locationOnConf, errorMessage, keyLen, keyLen+1)) return NULL;
    int* integerValue = malloc(sizeof(int));
    int len = (int) strlen(value);
    for (int i = 0; i < len; i++ ) *integerValue = *integerValue * 10 + value[i] - 48;

    makeLog(successMessage, NULL, NORMAL_LOG, SUCCESS);
    return integerValue;
}


char** getTargets(char* buffer) {
    char* locationOnConf = strstr(buffer, TARGETS);
    char errorMessage[20] = ERROR_MESSAGE;
    char successMessage[20] = SUCCESS_MESSAGE;
    strcat(errorMessage, "targets");
    strcat(successMessage, "targets");
    if (hasParseError(locationOnConf, errorMessage,0,0)) return NULL;

    char* splitter = "\n";
    char* target = strtok(locationOnConf, splitter);
    int counter = countTargets(locationOnConf);
    char** targets = malloc(sizeof(char*) * counter);
    counter = 0;

    while (target != NULL) {
        targets[counter] = target;
        target = strtok(NULL, splitter);
        counter++;
    }

    makeLog(successMessage, NULL, NORMAL_LOG, SUCCESS);
    return targets;
}

int countTargets(const char* locationOfTargets) {
    int counter = 0;
    for (int i = 0; locationOfTargets[i] != '\0'; i++) {
        if (locationOfTargets[i] == '\n') ++counter;
    }

    return counter;
}

int hasParseError(const char* locationOnConf, const char* message, int index1, int index2) {
    // check if locationConf.
    if (locationOnConf == NULL) return makeLog(message, PARSE_FAILED_NO_FIELD, NORMAL_LOG, ERROR);
    else if (locationOnConf[index1] == '\n') return makeLog(message, PARSE_FAILED_NOTHING, NORMAL_LOG, ERROR);
    else if (locationOnConf[index2] == '\n') return makeLog(message, PARSE_FAILED_EMPTY, NORMAL_LOG, ERROR);

    return 0;
}

