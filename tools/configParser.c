#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "configParser.h"
#include "logger.h"

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


void parseData(struct config *, const char *, size_t bufferSize);

char **getDependencies(const char *, char *, int*);

int countTargets(char *, int);

int getDependencyLength(const char *);

int getValueByKeyValidator(const char *, const char *, int, int);

void *getValueByKey(const char*, char *, int, size_t);

int getConfig(struct config *conf) {
    char* username = getlogin();
    char* configPath = malloc(sizeof(char) * (strlen(CONFIG_PATH) + strlen(username) + strlen("home") + 3));
    strcpy(configPath, "/home/");
    strcat(configPath, username);
    strcat(configPath, CONFIG_PATH);

    int confFD = open(configPath, O_RDONLY);
    char *buffer = NULL;
    // error handling.
    if (confFD == -1) return makeLog(FAILED_TO_OPEN, strerror(errno), NORMAL_LOG, ERROR);
    struct stat fileStats;

    if (lstat(configPath, &fileStats) == -1) return makeLog(FAILED_TO_READ_SIZE, strerror(errno), NORMAL_LOG, ERROR);
    // make the size of the buffer equal to the config file size.
    buffer = malloc(sizeof(char) * fileStats.st_size);
    // error handling.
    if (read(confFD, buffer, fileStats.st_size) == -1)
        return makeLog(FAILED_TO_READ, strerror(errno), NORMAL_LOG, ERROR);

    if (close(confFD)) return makeLog(FAILED_TO_CLOSE, strerror(errno), NORMAL_LOG, ERROR);

    makeLog(SUCCESS_LOAD, NULL, NORMAL_LOG, SUCCESS);
    parseData(conf, buffer, fileStats.st_size);
    free(configPath);
    free(buffer);
    return 0;
}

void parseData(struct config *conf, const char *buffer, size_t bufferSize) {
    conf->checkInterval = (int *) getValueByKey(buffer, CHECK_INTERVAL, 1, bufferSize);
    conf->parseInterval = (int *) getValueByKey(buffer, PARSE_INTERVAL, 1, bufferSize);
    conf->debugLog = (int *) getValueByKey(buffer, DEBUG, 1, bufferSize);
    conf->defaultDirPath = (char *) getValueByKey(buffer, DEFAULT_DIR_PATH, 0, bufferSize);
    conf->targetsPath = getDependencies(buffer, TARGETS, &conf->targetNumber);
    conf->check = getDependencies(buffer, CHECK, &conf->checkNumber);
}

void *getValueByKey(const char* buffer, char* key, int isInteger, size_t bufferSize) {
    char *tmp = NULL;
    tmp = malloc(sizeof(char) * (bufferSize));
    strncpy(tmp, buffer, bufferSize);

    char *locationOnConf = strstr(tmp, key);
    char errorMessage[50] = ERROR_MESSAGE;
    char successMessage[50] = SUCCESS_MESSAGE;
    strcat(errorMessage, key);
    strcat(successMessage, key);

    int keyLen = (int) strlen(key);
    if (getValueByKeyValidator(locationOnConf, errorMessage, keyLen, keyLen + 1)) return NULL;

    strtok(locationOnConf, " ");
    char *value = strtok(NULL, "\n");

    if (!isInteger) {
        if (getValueByKeyValidator(locationOnConf, errorMessage, keyLen, keyLen + 1)) return NULL;
        size_t valueLen = strlen(value) + 1;
        char *returnedValue = malloc(sizeof(char) * valueLen);
        strncpy(returnedValue, value, valueLen);
        makeLog(successMessage, NULL, NORMAL_LOG, SUCCESS);
        free(tmp);
        return returnedValue;
    }

    if (getValueByKeyValidator(locationOnConf, errorMessage, keyLen, keyLen + 1)) return NULL;
    long tmpInteger = strtol(value, &value, 10);
    int *integerValue = malloc(sizeof(int));
    *integerValue = (int) tmpInteger;
    free(tmp);
    makeLog(successMessage, NULL, NORMAL_LOG, SUCCESS);

    return integerValue;
}

char **getDependencies(const char *buffer, char *dependency, int* number) {
    int locationLen = getDependencyLength(strstr(buffer, dependency));
    char *locationOnConf = calloc(locationLen + 1, sizeof(char));
    strncpy(locationOnConf, strstr(buffer, dependency), locationLen);

    char errorMessage[20] = ERROR_MESSAGE;
    char successMessage[20] = SUCCESS_MESSAGE;
    strcat(errorMessage, dependency);
    strcat(successMessage, dependency);

    if (locationLen == 0) {
        makeLog(errorMessage, PARSE_FAILED_EMPTY, NORMAL_LOG, ERROR);
        return NULL;
    }

    char *splitter = "\n";
    int depCounter = countTargets(locationOnConf, locationLen);

    char *currentDependency = strtok(locationOnConf, splitter);
    char **dependencies = malloc(sizeof(char *) * (depCounter + 1));
    int saveCounter = 0;
    size_t tmpLen = 0;
    // erase garbage.
    currentDependency = strtok(NULL, splitter);
    while (currentDependency != NULL && saveCounter < depCounter) {
        tmpLen = strlen(currentDependency);
        dependencies[saveCounter] = calloc(tmpLen + 1, sizeof(char));
        strncpy(dependencies[saveCounter], currentDependency, tmpLen);
        currentDependency = strtok(NULL, splitter);
        saveCounter++;
    }
    *number = depCounter;

    free(locationOnConf);
    makeLog(successMessage, NULL, NORMAL_LOG, SUCCESS);
    return dependencies;
}

int countTargets(char *locationOfTargets, int len) {
    char* tmpLocation = calloc(len + 1, sizeof(char));
    strncpy(tmpLocation, locationOfTargets, len);
    char* tmp = strtok(tmpLocation, "\n");

    int counter = -1;
    while (tmp != NULL) {
        tmp = strtok(NULL, "\n");
        ++counter;
    }

    free(tmpLocation);
    return counter;
}

int getValueByKeyValidator(const char *locationOnConf, const char *message, int index1, int index2) {
    if (locationOnConf == NULL) return makeLog(message, PARSE_FAILED_NO_FIELD, NORMAL_LOG, ERROR);
    else if (locationOnConf[index1] == '\n') return makeLog(message, PARSE_FAILED_NOTHING, NORMAL_LOG, ERROR);
    else if (locationOnConf[index2] == '\n') return makeLog(message, PARSE_FAILED_EMPTY, NORMAL_LOG, ERROR);

    return 0;
}

int getDependencyLength(const char* dependency) {
    if (dependency == NULL) return 0;

    int len = 1;
    for (int ln = 1; dependency[ln] != '['; ln++) ++len;

    return len;
}

void freeDependencies(char** dependencies, int depNumber) {
    for (int currFree = 0; currFree < depNumber; currFree++) {
        if (dependencies[currFree] != NULL) free(dependencies[currFree]);
    }
    free(dependencies);
}

struct config *clearConfig(struct config *conf) {
    if (conf->checkInterval != NULL) free(conf->checkInterval);
    if (conf->parseInterval != NULL) free(conf->parseInterval);
    if (conf->debugLog != NULL) free(conf->debugLog);
    if (conf->defaultDirPath != NULL) free(conf->defaultDirPath);
    if (conf->targetsPath != NULL) freeDependencies(conf->targetsPath, conf->targetNumber);
    if (conf->check != NULL) freeDependencies(conf->check, conf->checkNumber);

    free(conf);
    return calloc(1, sizeof(struct config));
}


