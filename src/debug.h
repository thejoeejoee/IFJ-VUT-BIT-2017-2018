#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG

#define LOG_VERBOSITY_DEBUG 2
#define LOG_VERBOSITY_INFO 1

extern short log_verbosity;

short log_verbosity = LOG_VERBOSITY_DEBUG;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define _MESSAGE(verbosity, level_name, ...) if (log_verbosity >= verbosity) \
do {\
    fprintf(stderr, "%s [%s:%d:%s()]: ", level_name, __FILENAME__, __LINE__, __func__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
} while (0)

#define DEBUG(...) _MESSAGE(LOG_VERBOSITY_DEBUG, "DEBUG", __VA_ARGS__)

#define INFO(...) _MESSAGE(LOG_VERBOSITY_INFO, "INFO", __VA_ARGS__)

#else

#define DEBUG(...)
#define INFO(...)

#endif
#endif //_DEBUG_H

