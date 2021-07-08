#include <stdio.h>

#include "logger.h"

#define DEBUG_PREFIX "[DEBUG]"
#define NORMAL_PREFIX "[LOG]"

#define ERROR_PREFIX "[ERROR]"
#define SUCCESS_PREFIX "[SUCCESS]"
#define WARN_PREFIX "[WARN]"

#define LOG_FILE "../logs/logs.log"

void showLogs(const char*, const char*, const char*, const char*);
int saveLogs();

int makeLog(const char* message, const char* reason , int logType, int seriousness) {
    char* prefix;
    char* seriousnessType;
    if (logType == DEBUG_LOG) prefix = DEBUG_PREFIX;
    else prefix = NORMAL_PREFIX;

    if (seriousness == ERROR) seriousnessType = ERROR_PREFIX;
    else if (seriousness == SUCCESS) seriousnessType = SUCCESS_PREFIX;
    else seriousnessType = WARN_PREFIX;

    showLogs(message, prefix, seriousnessType, reason);

    if (seriousness == ERROR) return -1;

    return 0;
}

void showLogs(const char* message,
              const char* logTypePrefix,
              const char* seriousnessType,
              const char* reason) {
    if (reason) {
        printf("%s %s %s [reason] %s\n",logTypePrefix,seriousnessType, message, reason);
        return;
    }
    printf("%s %s %s\n",logTypePrefix,seriousnessType, message);

}

int saveLogs() {

}
