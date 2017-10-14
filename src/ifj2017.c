#include "ifj2017.h"

int stdin_stream() {
    return getchar();
}

int main(int argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);


    log_verbosity = LOG_VERBOSITY_WARNING;
    Parser* parser = parser_init(stdin_stream);

    if(!parser_parse(parser)) {
        ErrorReport report = parser->error_report;
        parser_free(&parser);
        exit_with_detail_information(report);
    }

    setbuf(stdout, NULL);
    code_generator_render(parser->code_constructor->generator, stdout);
    fflush(stdout);

    parser_free(&parser);

    return EXIT_SUCCESS;
}
