#ifndef FILE_SORTER_CORE_MEM_H
#define FILE_SORTER_CORE_MEM_H 1

#include <log/logger.h>

#define OUT_OF_MEMORY       "Out Of Memory."
#define FAILED_MEMORY_MSG   "Failed to allocate memory."

#define ALLOCATE_MEM(VAR, SIZE, SIZE_OF_TYPE) do {      \
            (VAR) = calloc(SIZE, SIZE_OF_TYPE);         \
            if ( (VAR) == NULL) {                       \
                make_log(FAILED_MEMORY_MSG,             \
                         OUT_OF_MEMORY,                 \
                         NORMAL_LOG,                    \
                         ERROR);                        \
                exit(1);                                \
            }                                           \
    } while(0)

#endif