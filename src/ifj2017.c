#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "common.h"

short log_verbosity = LOG_VERBOSITY_DEBUG;

int main(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    printf("Hello world, IFJ!\n");
    return EXIT_SUCCESS;
}
