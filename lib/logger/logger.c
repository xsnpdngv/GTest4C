// logger.c
#include "logger.h"
#include <stdio.h>

int loggerWriteLog(const char *message)
{
    return fprintf(stderr, "[LOG] %s\n", message);
}
