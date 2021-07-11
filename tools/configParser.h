#ifndef FILE_MANAGER_CONFIGPARSER_H
#define FILE_MANAGER_CONFIGPARSER_H 1

struct config {
    int* checkInterval;
    int* parseInterval;
    int* sortOnReboot;
    int* debugLog;
    char* defaultDirPath;
    char** targetsPath;
    char** check;
};

extern int getConfig(struct config* conf);
extern struct config* clearConfig(struct config* conf);

#endif
