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

    if (!parser_parse(parser))
        exit_with_detail_information(parser->error_report);

    return EXIT_SUCCESS;
}
