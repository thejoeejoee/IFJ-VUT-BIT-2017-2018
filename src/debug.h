#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG

#define LOG_VERBOSITY_DEBUG 2
#define LOG_VERBOSITY_INFO 1
#define LOG_VERBOSITY_WARNING 0

extern short log_verbosity;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define _MESSAGE(verbosity, level_name, ...) if (log_verbosity >= verbosity) \
do {\
    fprintf(stderr, "%s [%s:%d:%s()]: ", level_name, __FILENAME__, __LINE__, __func__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
} while (0)

#define LOG_DEBUG(...) _MESSAGE(LOG_VERBOSITY_DEBUG, "DEBUG", __VA_ARGS__)

#define LOG_INFO(...) _MESSAGE(LOG_VERBOSITY_INFO, "INFO", __VA_ARGS__)

#define LOG_WARNING(...) _MESSAGE(LOG_VERBOSITY_WARNING, "WARNING", __VA_ARGS__)

#define DEBUG_CODE(code) code

#else

#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define DEBUG_CODE(...)

#endif
#endif //_DEBUG_H

