#ifndef FILE_MANAGER_LOGGER_H
#define FILE_MANAGER_LOGGER_H 1

#define NORMAL_LOG 0
#define DEBUG_LOG 1

#define SUCCESS 2
#define ERROR 3
#define WARN 4

extern void makeLog(const char* message, int logType, int seriousness);

#endif
