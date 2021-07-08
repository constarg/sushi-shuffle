#include <stdio.h>

#include "logger.h"

#define DEBUG_PREFIX "[DEBUG]"
#define NORMAL_PREFIX "[LOG]"

#define ERROR_PREFIX "[ERROR]"
#define SUCCESS_PREFIX "[SUCCESS]"
#define WARN_PREFIX "[WARN]"

void showLogs(char*, char*,char*);

void makeLog(char* message, int logType, int seriousness) {
    char* prefix;
    char* seriousnessType;
    if (logType == DEBUG_LOG) prefix = DEBUG_PREFIX;
    else prefix = NORMAL_PREFIX;

    if (seriousness == ERROR) seriousnessType = ERROR_PREFIX;
    else if (seriousness == SUCCESS) seriousnessType = SUCCESS_PREFIX;
    else seriousnessType = WARN_PREFIX;

    showLogs(message, prefix, seriousnessType);
}

void showLogs(char* message, char* logTypePrefix, char* seriousnessType) {
    printf("%s %s %s\n",logTypePrefix,seriousnessType, message);
}

