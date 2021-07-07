#ifndef FILE_MANAGER_CONFIGPARSER_H
#define FILE_MANAGER_CONFIGPARSER_H 1

#define CONFIG_PATH "../config/config.conf"
#define TIMEOUT "timeout"
#define SORT_ON_REBOOT "sortOnReboot"
#define DEBUG "debugLog"
#define DEFAULT_DIR_PATH "default_dir_path"
#define TARGETS "[targets]"

struct config {
    int timeout;
    int sortOnReboot;
    int debug;
    char* defaultFolderPath;
    char** targetsPath;
};


extern int getConfig(struct config*);

#endif
