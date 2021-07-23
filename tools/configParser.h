#ifndef FILE_MANAGER_CONFIGPARSER_H
#define FILE_MANAGER_CONFIGPARSER_H 1

struct config {
    int *checkInterval;
    int *parseInterval;
    int *debugLog;
    char *defaultDirPath;
    char **targetsPath;
    char **check;
    int checkNumber;
    int targetNumber;
};

extern int getConfig(struct config *conf);

extern struct config *clearConfig(struct config *conf);

#endif
