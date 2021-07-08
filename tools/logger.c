#include <stdio.h>

#include "logger.h"

#define DEBUG_PREFIX "[DEBUG]"
#define NORMAL_PREFIX "[LOG]"

#define ERROR_PREFIX "[ERROR]"
#define SUCCESS_PREFIX "[SUCCESS]"
#define WARN_PREFIX "[WARN]"

void showLogs(const char*, const char*, const char*);

void makeLog(const char* message, int logType, int seriousness) {
    char* prefix;
    char* seriousnessType;
    if (logType == DEBUG_LOG) prefix = DEBUG_PREFIX;
    else prefix = NORMAL_PREFIX;

    if (seriousness == ERROR) seriousnessType = ERROR_PREFIX;
    else if (seriousness == SUCCESS) seriousnessType = SUCCESS_PREFIX;
    else seriousnessType = WARN_PREFIX;

    showLogs(message, prefix, seriousnessType);
}

void showLogs(const char* message, const char* logTypePrefix, const char* seriousnessType) {
    printf("%s %s %s\n",logTypePrefix,seriousnessType, message);
}

