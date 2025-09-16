// logger.c
#include "logger.h"
#include <stdio.h>

int loggerWriteLog(const char *message)
{
    return printf("[LOG] %s\n", message);
}
