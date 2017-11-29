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
//    while(
//    code_optimizer_remove_unused_variables(parser->optimizer, true) ||
//    code_optimizer_remove_unused_functions(parser->optimizer));

    while(
            code_optimizer_peep_hole_optimization(parser->optimizer)
            );

//    code_optimizer_remove_unused_variables(parser->optimizer, false);

    code_optimizer_split_code_to_graph(parser->optimizer);
//    oriented_graph_print(parser->optimizer->code_graph);
    code_optimizer_propate_constants_optimization(parser->optimizer);
    for(unsigned int i = 0; i < parser->optimizer->code_graph->nodes_count; i++) {
        CodeBlock* code_block = (CodeBlock*) oriented_graph_node(parser->optimizer->code_graph, i);

        CodeInstruction* instruction = code_block->instructions;
        printf("\nBlock %d - count %d\n", code_block->base.id, code_block->instructions_count);
        for(unsigned int j = 0; j < code_block->instructions_count; j++) {
            char* rendered = code_instruction_render(instruction);
            printf("%s\n", rendered);
            memory_free(rendered);
            instruction = instruction->next;
        }
    }

//    code_generator_render(parser->code_constructor->generator, stdout);
    fflush(stdout);

    parser_free(&parser);

    memory_manager_exit(&memory_manager);

    return EXIT_SUCCESS;
}
