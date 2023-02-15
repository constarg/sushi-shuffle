#include <stdio.h>

#include <logger.h>


void logger(const char *msg, int type)
{
    switch (type) {
        case ERROR:
            printf("[ERROR] %s\n", msg);
            break;
        case WAR:
            printf("[WARNING] %s\n", msg);
            break;
        case LOG:
            printf("[LOG] %s\n", msg);
    }
}
