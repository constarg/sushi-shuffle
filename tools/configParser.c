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


void parseData(struct config *, char *);

char **getDependencies(char *, char *);

int countTargets(const char *);

int hasParseError(const char *, const char *, int, int);

void *getValueByKey(char[], char *, int);

int getConfig(struct config *conf) {
    char* username = getlogin();
    char* configPath = alloca(sizeof(char) * (strlen(CONFIG_PATH) + strlen(username) + strlen("home")));
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
    parseData(conf, buffer);
    free(buffer);
    return 0;
}

void parseData(struct config *conf, char *buffer) {
    conf->checkInterval = (int *) getValueByKey(buffer, CHECK_INTERVAL, 1);
    conf->parseInterval = (int *) getValueByKey(buffer, PARSE_INTERVAL, 1);
    conf->debugLog = (int *) getValueByKey(buffer, DEBUG, 1);
    conf->defaultDirPath = (char *) getValueByKey(buffer, DEFAULT_DIR_PATH, 0);
    conf->targetsPath = getDependencies(buffer, TARGETS);
    conf->check = getDependencies(buffer, CHECK);
}

void *getValueByKey(char buffer[], char key[], int isInteger) {
    char *tmp = NULL;
    tmp = malloc(sizeof(char) * strlen(buffer));
    strcpy(tmp, buffer);
    char *locationOnConf = strstr(tmp, key);
    char errorMessage[50] = ERROR_MESSAGE;
    char successMessage[50] = SUCCESS_MESSAGE;
    strcat(errorMessage, key);
    strcat(successMessage, key);
    int keyLen = (int) strlen(key);

    if (hasParseError(locationOnConf, errorMessage, keyLen, keyLen + 1)) return NULL;

    strtok(locationOnConf, " ");
    char *value = strtok(NULL, "\n");

    if (!isInteger) {
        if (hasParseError(locationOnConf, errorMessage, keyLen, keyLen + 1)) return NULL;
        char *returnedValue = malloc(sizeof(char) * strlen(value));
        strcpy(returnedValue, value);
        makeLog(successMessage, NULL, NORMAL_LOG, SUCCESS);
        free(tmp);
        return returnedValue;
    }
    if (hasParseError(locationOnConf, errorMessage, keyLen, keyLen + 1)) return NULL;
    long tmpInteger = strtol(value, &value, 10);
    int *integerValue = malloc(sizeof(int));
    *integerValue = (int) tmpInteger;
    free(tmp);
    makeLog(successMessage, NULL, NORMAL_LOG, SUCCESS);
    return integerValue;
}

char **getDependencies(char *buffer, char *dependency) {
    char *locationOnConf = strstr(buffer, dependency);
    char errorMessage[20] = ERROR_MESSAGE;
    char successMessage[20] = SUCCESS_MESSAGE;
    strcat(errorMessage, dependency);
    strcat(successMessage, dependency);
    if (hasParseError(locationOnConf, errorMessage, 0, 0)) return NULL;

    char *splitter = "\n";
    int counter = countTargets(locationOnConf);
    char *currentDependency = strtok(locationOnConf, splitter);

    char **dependencies = malloc(sizeof(char *) * counter);
    counter = 0;

    while (currentDependency != NULL) {
        dependencies[counter] = currentDependency;
        currentDependency = strtok(NULL, splitter);
        counter++;
    }

    makeLog(successMessage, NULL, NORMAL_LOG, SUCCESS);
    return dependencies;
}

int countTargets(const char *locationOfTargets) {
    int counter = 0;
    for (int i = 0; locationOfTargets[i] != '\0'; i++) {
        if (locationOfTargets[i] == '\n') ++counter;
    }

    return counter;
}

int hasParseError(const char *locationOnConf, const char *message, int index1, int index2) {
    if (locationOnConf == NULL) return makeLog(message, PARSE_FAILED_NO_FIELD, NORMAL_LOG, ERROR);
    else if (locationOnConf[index1] == '\n') return makeLog(message, PARSE_FAILED_NOTHING, NORMAL_LOG, ERROR);
    else if (locationOnConf[index2] == '\n') return makeLog(message, PARSE_FAILED_EMPTY, NORMAL_LOG, ERROR);

    return 0;
}

struct config *clearConfig(struct config *conf) {
    if (conf->checkInterval != NULL) free(conf->checkInterval);
    if (conf->parseInterval != NULL) free(conf->parseInterval);
    if (conf->debugLog != NULL) free(conf->debugLog);
    if (conf->defaultDirPath != NULL) free(conf->defaultDirPath);
    if (conf->targetsPath != NULL) free(conf->targetsPath);
    if (conf->check != NULL) free(conf->check);

    free(conf);
    return malloc(sizeof(struct config));
}


