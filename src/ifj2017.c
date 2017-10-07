#include "ifj2017.h"

int stdin_stream() {
    return getchar();
}

int main(int argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);

    Parser* parser = parser_init(stdin_stream);

    if(!parser_parse(parser))
        exit_with_detail_information(parser->error_report);

    code_generator_render(parser->code_constructor->generator, stdout);

    return EXIT_SUCCESS;
}
