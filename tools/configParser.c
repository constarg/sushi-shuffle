#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>

#include "configParser.h"

int getTimeout(const char*);
int getSortOnReboot(char* buffer);
int getDebug(char*);
char* getDefaultDirPath(char*);
char** getTargets(char*);
int getLength(const char*);
int countTargets(const char* locationOfTargets);

int getConfig(struct config* conf) {
    // get the file descriptor of config file.
    int confFD = open(CONFIG_PATH, O_RDONLY);
    char *buffer = NULL;

    if (confFD == -1) return 0;

    struct stat fileStats;

    if (lstat(CONFIG_PATH, &fileStats) == -1) {
        perror("lstat");
        // toDO log it using logger.
        return 0;
    }
    // make the size of the buffer equal to the config file size.
    buffer = malloc(sizeof(char) * fileStats.st_size);
    read(confFD, buffer, fileStats.st_size - 1);

    getTargets(buffer);

    free(buffer);
    return 1;
}

int parseData(struct config* conf, char* buffer) {



    return 1;
}

int getTimeout(const char* buffer) {
    char* locationOnConf = strstr(buffer, TIMEOUT);
    if (locationOnConf[0] == '#') return -1;

    int timeout = 0;
    for (int i = 8; locationOnConf[i] != '\n'; i++) timeout = timeout * 10 + locationOnConf[i] - 48;

    return timeout;
}

int getSortOnReboot(char* buffer) {
    char* locationOnConf = strstr(buffer, SORT_ON_REBOOT);
    if (locationOnConf[0] == '#') return -1;

    return (int) locationOnConf[13] - 48;
}


int getDebug(char* buffer) {
    char* locationOnConf = strstr(buffer, DEBUG);
    if (locationOnConf[0] == '#') return -1;

    return locationOnConf[9] - 48;
}

char* getDefaultDirPath(char* buffer) {
    char* locationOnConf = strstr(buffer, DEFAULT_DIR_PATH);
    if (locationOnConf[0] == '#') return NULL;

    int len = getLength(locationOnConf);
    char* defaultDir = malloc(sizeof(char) * len);
    for (int i = 17; i < len; i++) defaultDir[i - 17] = locationOnConf[i];

    return defaultDir;
}

char** getTargets(char* buffer) {
    char* locationOnConf = strstr(buffer, TARGETS);
    char* splitter = "\n";
    char* target = strtok(locationOnConf, splitter);
    int counter = countTargets(locationOnConf);
    char** targets = malloc(sizeof(char*) * counter);
    counter = 0;

    while (target != NULL) {
        if (target == TARGETS || target[0] == "#") continue;

        targets[counter] = target;
        target = strtok(NULL, splitter);
        counter++;
    }

    for (int i = 0; i < counter; i++) printf("%s",targets[i]);


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