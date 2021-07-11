#include <malloc.h>
#include <pthread.h>
#include <unistd.h>

#include "manager.h"
#include "../tools/configParser.h"
#include "../tools/logger.h"

#define TRUE 1

#define PARSER_FAILED "Parser Failed"
#define PARSE_INTERVAL_NOT_FOUND "Parse interval not found, set default"

#define DEFAULT_PARSE_INTERVAL 5000

_Noreturn void* parse(void*);
void* moveToDir(void*);

void run() {
    struct config* conf = malloc(sizeof(struct config));
    pthread_t moveToDirThread;
    pthread_t parseThread;

    pthread_create(&parseThread, NULL, parse, conf);
    //pthread_create(&moveToDirThread, NULL, moveToDir, conf);
    pthread_join(parseThread, NULL);
}

void* parse(void* conf) {
    struct config* currConf = conf;
    // parse the config file using parser.

    while (TRUE) {
        currConf = clearConfig(currConf);
        if (getConfig(conf) == -1) makeLog(PARSER_FAILED, NULL, NORMAL_LOG, WARN);

        if (currConf -> parseInterval == NULL) {
            sleep(DEFAULT_PARSE_INTERVAL);
            if (*(currConf -> debugLog) == 1) makeLog(PARSE_INTERVAL_NOT_FOUND, NULL, DEBUG_LOG, WARN);
            continue;
        }
        sleep(*(currConf -> parseInterval));
    }
}


void* moveToDir(void* conf) {


}
