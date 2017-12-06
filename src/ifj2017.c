#include <signal.h>
#include "ifj2017.h"
#include "code_optimizer.h"
#include "code_optimizer_expr.h"

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

    // optimized redundant type casts
    code_optimizer_optimize_type_casts(parser->optimizer);
    // updates all stats about occurrences, literal expressions, label occurrences etc.
    code_optimizer_update_meta_data(parser->optimizer);

    // first PH iterations (without advanced)
    while(
            code_optimizer_peep_hole_optimization(parser->optimizer)
            );

    bool expr_interpreted;
    do {
        // propagating constants into code blocks
        expr_interpreted = false;
        code_optimizer_split_code_to_graph(parser->optimizer);
        code_optimizer_update_meta_data(parser->optimizer);
        expr_interpreted |= code_optimizer_propagate_constants_optimization(parser->optimizer);
        code_optimizer_update_meta_data(parser->optimizer);
        // partial eval constant expressions
        expr_interpreted |= code_optimizer_literal_expression_eval_optimization(parser->optimizer);
    } while(expr_interpreted);

    // remove redundant instruction after constant propagation & expr eval
    code_optimizer_remove_instructions_without_effect_optimization(parser->optimizer);

    // hard remove all unused symbols
    while(
            code_optimizer_remove_unused_variables(parser->optimizer, true, false) ||
            code_optimizer_remove_unused_functions(parser->optimizer));

    // setup advanced PH patterns (with metaflags destruction)
    code_optimizer_add_advance_peep_hole_patterns(parser->optimizer);
    while(
            code_optimizer_peep_hole_optimization(parser->optimizer)
            );

    // gently remove all unused symbols (with temps keep)
    code_optimizer_remove_unused_variables(parser->optimizer, false, true);
    // eval all expression partials after constants were propagated
    code_optimizer_optimize_partial_expression_eval(parser->optimizer);
    // hard core PH
    while(
            code_optimizer_peep_hole_optimization(parser->optimizer)
            );


    code_optimizer_update_meta_data(parser->optimizer);
    code_optimizer_split_code_to_graph(parser->optimizer);
    code_optimizer_update_meta_data(parser->optimizer);
    code_optimizer_propagate_constants_optimization(parser->optimizer);
    code_optimizer_optimize_jumps(parser->optimizer);


    while(code_optimizer_peep_hole_optimization(parser->optimizer));
    code_optimizer_optimize_comparements(parser->optimizer);


    code_optimizer_update_meta_data(parser->optimizer);
    code_optimizer_split_code_to_graph(parser->optimizer);
    code_optimizer_update_meta_data(parser->optimizer);
    code_optimizer_propagate_constants_optimization(parser->optimizer);
    code_optimizer_optimize_jumps(parser->optimizer);

    while(code_optimizer_peep_hole_optimization(parser->optimizer));

    // gently remove all unused symbols (with temps keep)
    code_optimizer_update_meta_data(parser->optimizer);
    code_optimizer_remove_unused_variables(parser->optimizer, false, true);


    code_generator_render(parser->code_constructor->generator, stdout);
    fflush(stdout);

    parser_free(&parser);
    memory_manager_exit(&memory_manager);
    return EXIT_SUCCESS;
}
