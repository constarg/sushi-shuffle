#include <stdio.h>

#include <log/logger.h>

#define DEBUG_PREFIX    "[DEBUG]"
#define NORMAL_PREFIX   "[LOG]"

#define ERROR_PREFIX    "[ERROR]"
#define SUCCESS_PREFIX  "[SUCCESS]"
#define WARN_PREFIX     "[WARN]"

static inline void show_logs(const char *message,
               const char *log_type_prefix,
               const char *seriousness_type,
               const char *reason) {
    if (reason) {
        printf("%s %s %s [reason] %s\n", log_type_prefix, seriousness_type, message, reason);
        return;
    }
    printf("%s %s %s\n", log_type_prefix, seriousness_type, message);

}

int make_log(const char *message, const char *reason, int log_type, int seriousness) {
    char *prefix;
    char *seriousness_type;
    if (log_type == DEBUG_LOG) prefix = DEBUG_PREFIX;
    else prefix = NORMAL_PREFIX;

    if (seriousness == ERROR) seriousness_type = ERROR_PREFIX;
    else if (seriousness == SUCCESS) seriousness_type = SUCCESS_PREFIX;
    else seriousness_type = WARN_PREFIX;

    show_logs(message, prefix, seriousness_type, reason);

    if (seriousness == ERROR) return -1;

    return 0;
}


