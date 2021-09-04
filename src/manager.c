#include <sys/types.h>
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <alloca.h>

#include <manager.h>
#include <config/config_parser.h>
#include <log/logger.h>

#define TRUE                        1

#define PARSER_FAILED               "Parser Failed"
#define PARSE_INTERVAL_NOT_FOUND    "Parse interval not found, set default"
#define FAILED_TO_OPEN_DIR          "Parser Failed to open a check dir."
#define FAILED_TO_INITIALIZE_MUTEX  "Failed to initialize mutex."
#define FAILED_TO_CREATE_THREAD     "Failed to create thread."
#define FAILED_TO_MOVE_FILE         "Failed to move file."

#define SUCCESS_MOVE                "File moved successfully to target directory."

#define DEFAULT_PARSE_INTERVAL      5000
#define INITIAL_CHECK_WAIT          5


static struct config *conf;


void run() {

}