#ifndef FILE_MANAGER_CONFIGPARSER_H
#define FILE_MANAGER_CONFIGPARSER_H 1

struct config {
    int* timeout;
    int* sortOnReboot;
    int* debug;
    char* defaultFolderPath;
    char** targetsPath;
    char** check;
};

extern int getConfig(struct config* conf);

#endif
