#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <alloca.h>
#include <unistd.h>
#include <string.h>

#include "configParser.h"
#include "logger.h"

#define CONFIG_PATH "../config/config.conf"
#define TIMEOUT "timeout"
#define SORT_ON_REBOOT "sortOnReboot"
#define DEBUG "debugLog"
#define DEFAULT_DIR_PATH "default_dir_path"
#define TARGETS "[targets]"
#define ERROR_MESSAGE "Failed to parse "
#define SUCCESS_MESSAGE "Successfully parse "

void parseData(struct config*, char*);
char** getTargets(char*);
int countTargets(const char*);
int hasParseError(const char*, const char*);
void* getValueByKey(const char*, char*,int);

int getConfig(struct config* conf) {
    // get the file descriptor of config file.
    int confFD = open(CONFIG_PATH, O_RDONLY);
    char *buffer = NULL;

    if (confFD == -1) return 0;

    struct stat fileStats;

    if (lstat(CONFIG_PATH, &fileStats) == -1) {
        makeLog("Failed to read config size.", NORMAL_LOG, ERROR);
        return -1;
    }
    // make the size of the buffer equal to the config file size.
    buffer = malloc(sizeof(char) * fileStats.st_size);
    if (read(confFD, buffer, fileStats.st_size) == -1) {
        makeLog("Failed to read config.", NORMAL_LOG, ERROR);
        return -1;
    }
    parseData(conf, buffer);

    free(buffer);
    makeLog("Successfully load config", NORMAL_LOG, SUCCESS);
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
    char* tmp = alloca(sizeof(char) * strlen(buffer));
    strcpy(tmp, buffer);
    char* locationOnConf = strstr(tmp, key);
    char errorMessage[50] = ERROR_MESSAGE;
    char successMessage[50] = SUCCESS_MESSAGE;
    strcat(errorMessage, key);
    strcat(successMessage, key);

    if (hasParseError(locationOnConf, errorMessage)) return NULL;

    strtok(locationOnConf, " ");
    char* value = strtok(NULL, "\n");

    if (!isInteger) {
        if (hasParseError(value, errorMessage)) return NULL;
        return value;
    }
    if (hasParseError(value, errorMessage)) return NULL;
    int* integerValue = malloc(sizeof(int));
    int len = (int) strlen(value);
    for (int i = 0; i < len; i++ ) *integerValue = *integerValue * 10 + value[i] - 48;

    makeLog(successMessage, NORMAL_LOG, SUCCESS);
    return integerValue;
}


char** getTargets(char* buffer) {
    char* locationOnConf = strstr(buffer, TARGETS);
    int error = hasParseError(locationOnConf, "Failed to parse targets");
    if (error) return NULL;

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

    makeLog("Successfully parse targets", NORMAL_LOG, SUCCESS);
    return targets;
}

int countTargets(const char* locationOfTargets) {
    int counter = 0;
    for (int i = 0; locationOfTargets[i] != '\0'; i++) {
        if (locationOfTargets[i] == '\n') ++counter;
    }

    return counter;
}

int hasParseError(const char* locationOnConf, const char* message) {

    if (locationOnConf == NULL) {
        makeLog(message, NORMAL_LOG, ERROR);
        return 1;
    }

    return 0;
}

