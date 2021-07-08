#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
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

void parseData(struct config*, char*);
int getTimeout(char*);
int getSortOnReboot(char*);
int getDebug(char*);
char* getDefaultDirPath(char*);
char** getTargets(char*);
int getLength(const char*);
int countTargets(const char*);
int hasParseError(const char*,char*, int, int);

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
    makeLog("Config has loaded successfully", NORMAL_LOG, SUCCESS);
    return 1;
}

void parseData(struct config* conf, char* buffer) {
    conf -> timeout = getTimeout(buffer);
    conf -> sortOnReboot = getSortOnReboot(buffer);
    conf -> debug = getDebug(buffer);
    conf -> defaultFolderPath = getDefaultDirPath(buffer);
    conf -> targetsPath = getTargets(buffer);
}

int getTimeout(char* buffer) {
    char* locationOnConf = strstr(buffer, TIMEOUT);
    int error = hasParseError(locationOnConf, "Failed to parse timeout",7, 8);

    if (error) return -1;

    int timeout = 0;
    for (int i = 8; locationOnConf[i] != '\n'; i++) timeout = timeout * 10 + locationOnConf[i] - 48;

    makeLog("Timeout parsed successfully", NORMAL_LOG, SUCCESS);
    return timeout;
}

int getSortOnReboot(char* buffer) {
    char* locationOnConf = strstr(buffer, SORT_ON_REBOOT);
    int error = hasParseError(locationOnConf, "Failed to parse sortOnReboot", 12, 13);
    if (error) return -1;

    makeLog("SortOnReboot parsed successfully", NORMAL_LOG, SUCCESS);
    return (int) locationOnConf[13] - 48;
}

int getDebug(char* buffer) {
    char* locationOnConf = strstr(buffer, DEBUG);
    int error = hasParseError(locationOnConf, "Failed to parse debugLog",8, 9);
    if (error) return -1;

    makeLog("DebugLog parsed successfully", NORMAL_LOG, SUCCESS);
    return (int) locationOnConf[9] - 48;
}

char* getDefaultDirPath(char* buffer) {
    char* locationOnConf = strstr(buffer, DEFAULT_DIR_PATH);
    int error = hasParseError(locationOnConf,"Failed to parse default_dir_path",16, 17);

    if (error) return NULL;

    int len = getLength(locationOnConf);
    char* defaultDir = malloc(sizeof(char) * len);
    for (int i = 17; i < len; i++) defaultDir[i - 17] = locationOnConf[i];

    makeLog("Default_dir_path parsed successfully", NORMAL_LOG, SUCCESS);
    return defaultDir;
}

char** getTargets(char* buffer) {
    char* locationOnConf = strstr(buffer, TARGETS);
    int error = hasParseError(locationOnConf, "Failed to parse targets", 0, 0);
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

    makeLog("Targets parsed successfully", NORMAL_LOG, SUCCESS);
    return targets;
}

int getLength(const char* line) {
    int len = 0;
    for (int i = 0; line[i] != '\n'; i++) ++len;

    return len;
}

int countTargets(const char* locationOfTargets) {
    int counter = 0;
    for (int i = 0; locationOfTargets[i] != '\0'; i++) {
        if (locationOfTargets[i] == '\n') ++counter;
    }

    return counter;
}

int hasParseError(const char* locationOnConf,
                  char* message, int index1, int index2) {

    if (locationOnConf == NULL || (locationOnConf[index1] == '\n' || locationOnConf[index2] == ' ') ) {
        makeLog(message, NORMAL_LOG, ERROR);
        return 1;
    }

    return 0;
}