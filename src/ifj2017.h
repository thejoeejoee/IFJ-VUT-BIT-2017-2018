#ifndef _IFJ2017
#define _IFJ2017

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "common.h"
#include "parser.h"

#ifndef NDEBUG
short log_verbosity = LOG_VERBOSITY_DEBUG;
#else
short log_verbosity = LOG_VERBOSITY_WARNING;
#endif

int stdin_stream();

int main(int argc, char* argv[]);

#endif