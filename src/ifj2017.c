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

    CodeOptimizer* optimizer = code_optimizer_init(parser->code_constructor->generator,
                                                   parser->parser_semantic->temp_variable1,
                                                   parser->parser_semantic->temp_variable2,
                                                   parser->parser_semantic->temp_variable3,
                                                   parser->parser_semantic->temp_variable4,
                                                   parser->parser_semantic->temp_variable5);

    bool optimized = false;
    do {
        optimized = /*code_optimizer_remove_unused_variables(optimizer) || code_optimizer_remove_unused_functions(optimizer) ||*/ code_optimizer_peep_hole_optimization(optimizer);
    } while(optimized);
    code_optimizer_free(&optimizer);

    code_generator_render(parser->code_constructor->generator, stdout);

    fflush(stdout);

    parser_free(&parser);

    memory_manager_exit(&memory_manager);

    return EXIT_SUCCESS;
}
