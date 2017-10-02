#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "common.h"
#include "parser.h"

short log_verbosity = LOG_VERBOSITY_DEBUG;

int stdin_stream() {
    return getchar();
}

int main(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    Parser* parser = parser_init(stdin_stream);

    parser_parse_program(parser);
    return EXIT_SUCCESS;
}
