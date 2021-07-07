#include <stdio.h>
#include <malloc.h>
#include "tools/configParser.h"



int main() {
    struct config *test = malloc(sizeof(struct config));

    getConfig(test);
    return 0;
}