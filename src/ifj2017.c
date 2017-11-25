#include "ifj2017.h"
#include "code_optimizer.h"

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

    code_optimizer_update_meta_data(parser->optimizer);

    while(
    code_optimizer_remove_unused_variables(parser->optimizer, true) ||
    code_optimizer_remove_unused_functions(parser->optimizer));

    while(
            code_optimizer_peep_hole_optimization(parser->optimizer)
            );

    code_optimizer_remove_unused_variables(parser->optimizer, false);
    code_generator_render(parser->code_constructor->generator, stdout);

    fflush(stdout);

    parser_free(&parser);

    memory_manager_exit(&memory_manager);

    return EXIT_SUCCESS;
}
