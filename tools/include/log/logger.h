#ifndef FILE_MANAGER_LOGGER_H
#define FILE_MANAGER_LOGGER_H 1

#define NORMAL_LOG  0
#define DEBUG_LOG   1

#define SUCCESS     2
#define ERROR       3
#define WARN        4

extern int make_log(const char *message, const char *reason, int log_type, int seriousness);

#endif
