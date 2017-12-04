#include "code_optimizer.h"
#include "memory.h"
#include "meta_data_constants_tables_stack.h"
#include "meta_data_cycled_blocks_mod_vars.h"

CodeOptimizer* code_optimizer_init(CodeGenerator* generator, SymbolVariable* temp1, SymbolVariable* temp2,
                                   SymbolVariable* temp3,
                                   SymbolVariable* temp4, SymbolVariable* temp5, SymbolVariable* temp6) {
    NULL_POINTER_CHECK(generator, NULL);
    NULL_POINTER_CHECK(temp1, NULL);
    NULL_POINTER_CHECK(temp2, NULL);
    NULL_POINTER_CHECK(temp3, NULL);
    NULL_POINTER_CHECK(temp4, NULL);
    NULL_POINTER_CHECK(temp5, NULL);
    NULL_POINTER_CHECK(temp6, NULL);


    CodeOptimizer* optimizer = memory_alloc(sizeof(CodeOptimizer));
    optimizer->variables_meta_data = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(VariableMetaData),
                                                       &init_variable_meta_data, NULL);

    optimizer->functions_meta_data = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(FunctionMetaData),
                                                       &init_function_meta_data,
                                                       &free_function_meta_data);

    optimizer->labels_meta_data = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(LabelMetaData),
                                                    &init_label_meta_data, NULL);

    optimizer->code_graph = oriented_graph_init(sizeof(CodeBlock), &init_code_block, &free_code_block);

    optimizer->generator = generator;
    optimizer->temp1 = temp1;
    optimizer->temp2 = temp2;
    optimizer->temp3 = temp3;
    optimizer->temp4 = temp4;
    optimizer->temp5 = temp5;
    optimizer->temp6 = temp6;

    optimizer->interpreter = interpreter_init(optimizer->temp1);

    llist_init(&optimizer->peep_hole_patterns, sizeof(PeepHolePattern), &init_peep_hole_pattern,
               &free_peep_hole_pattern, NULL);

    // adding peephole patterns
    /* Deleting unused frame
     * CREATEFRAME
     * PUSHFRAME        => E
     * POPFRAME
     */
    PeepHolePattern* pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_CREATE_FRAME, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_FRAME, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_FRAME, NULL, NULL, NULL, 0, 0, 0);

    /* Deleting unused jump on label, which is used only once
     * JUMP <a>         => E
     * LABEL <a>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP, "&a", NULL, NULL, 1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_LABEL, "&a", NULL, NULL, 1, 0, 0);

    /* Deleting unused jump to label, which is used not only once
     * JUMP <a>         => LABEL <a>
     * LABEL <a>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP, "&a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_LABEL, "&a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_LABEL, "&a", NULL, NULL);

    return optimizer;
}

void code_optimizer_add_advance_peep_hole_patterns(CodeOptimizer* optimizer) {
    NULL_POINTER_CHECK(optimizer,);

    /* Use normal operations with memory instead of stack if there is only 2 operands and result
     * is then stored in variable.
     * PUSH <a>         => OP <b> <a> <c>
     * PUSH <c>
     * OPS
     * POP <b>
     */
    PeepHolePattern* pattern = NULL;
    TypeInstruction stack_operations_instructions[] = {I_ADD_STACK, I_SUB_STACK, I_MUL_STACK, I_DIV_STACK,
                                                       I_LESSER_THEN_STACK, I_GREATER_THEN_STACK, I_EQUAL_STACK,
                                                       I_AND_STACK, I_OR_STACK, I_NOT_STACK};

    TypeInstruction operations_instructions[] = {I_ADD, I_SUB, I_MUL, I_DIV, I_LESSER_THEN, I_GREATER_THEN, I_EQUAL,
                                                 I_AND, I_OR, I_NOT};
    const int operations_count = (sizeof(operations_instructions) / sizeof(operations_instructions[0]));

    for(int i = 0; i < operations_count; i++) {
        pattern = code_optimizer_new_ph_pattern(optimizer);
        code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
        code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "c", NULL, NULL, -1, 0, 0);
        code_optimizer_add_matching_instruction_to_ph_pattern(pattern, stack_operations_instructions[i], NULL, NULL,
                                                              NULL, 0, 0, 0);
        code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!b", NULL, NULL, -1, 0, 0);

        code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, operations_instructions[i], "!b", "a", "c");
    }

    /* Remove concatinating with empty string
     * PUSH string@         => PUSH <a>
     * PUSH <a>
     * POP <b>
     * POP <c>
     * CONCAT <b> <c> <b>
     * PUSH <b>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "]_", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!c", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_CONCAT_STRING, "!b", "!c", "!b", -1, -1, -1);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "!b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL);

    /* Remove concatinating with empty string
     * PUSH string@         => PUSH <a>
     * MOVE <b> <a>
     * POP <c>
     * CONCAT <b> <c> <b>
     * PUSH <b>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "]_", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "!b", "a", NULL, -1, -1, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!c", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_CONCAT_STRING, "!b", "!c", "!b", -1, -1, -1);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "!b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL);

    /* Remove concatinating with empty string (reversed operands)
     * PUSH <a>             => PUSH <a>
     * PUSH string@
     * POP <b>
     * POP <c>
     * CONCAT <b> <c> <b>
     * PUSH <b>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "]_", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!c", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_CONCAT_STRING, "!b", "!c", "!b", -1, -1, -1);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "!b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL);

    /* Remove concatinating with empty string (reversed operands)
     * PUSH <a>             => PUSH <a>
     * MOVE <b> string@
     * POP <c>
     * CONCAT <b> <c> <b>
     * PUSH <b>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "!b", "]_", NULL, -1, -1, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!c", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_CONCAT_STRING, "!b", "!c", "!b", -1, -1, -1);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "!b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL);

    /* Remove concatinating with empty string (resolving for example a = "" + b + "xx"), it's
     * not first operation to be resolved in expression
     * PUSH string@         => PUSH <b>
     * POP <b>
     * POP <c>
     * CONCAT <b> <c> <b>
     * PUSH <b>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "]_", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!c", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_CONCAT_STRING, "!b", "!c", "!b", -1, -1, -1);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "!b", NULL, NULL, -1, 0, 0);

    /* Remove concatinating with empty string (resolving for example a = "" + b + "xx"), it's
     * not first operation to be resolved in expression
     * MOVE <b> string@     => PUSH <b>
     * POP <c>
     * CONCAT <b> <c> <b>
     * PUSH <b>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "!b", "]_", NULL, -1, -1, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!c", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_CONCAT_STRING, "!b", "!c", "!b", -1, -1, -1);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "!b", NULL, NULL, -1, 0, 0);

    /* Shorten print
     * MOVE <temp_1> <b>    => WRITE <b>
     * WRITE <temp_1>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "1a", "b", NULL, -1, -1, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_WRITE, "1a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_WRITE, "b", NULL, NULL);


    /* Double not on stack
     * NOT              => E
     * NOT
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_NOT_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_NOT_STACK, NULL, NULL, NULL, 0, 0, 0);

    /* Double not on stack
     * PUSH true       => E
     * ANDS
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "<", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_AND_STACK, NULL, NULL, NULL, 0, 0, 0);

    /* Double not on stack
     * PUSH true       => E
     * EQS
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "<", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_EQUAL_STACK, NULL, NULL, NULL, 0, 0, 0);

    /* Pattern expanding to always true
     * PUSH <a>       => PUSH true
     * PUSH <a>
     * NOTS
     * ORS
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "foobar", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "foobar", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_NOT_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_OR_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "<true", NULL, NULL);

    /* Pattern expanding to always true
     * PUSH false       => NOTS
     * EQS
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, ">", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_EQUAL_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_NOT_STACK, NULL, NULL, NULL);

    /* Pattern expanding to always true
     * LABEL <label>       => E
     */
//    pattern = code_optimizer_new_ph_pattern(optimizer);
//    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_LABEL, "&l", NULL, NULL, 0, 0, 0);

    /* Pattern expanding to always true
     * JUMP <label>       => JUMP <label>
     * JUMP <label>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP, "&l", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP, "&l", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_JUMP, "&l", NULL, NULL);

    /* Shorten print (can reduce variable in general)
     * MOVE <a> <b>    => MOVE <a> <b>
     * WRITE <a>       => WRITE <b>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "!a", "b", NULL, -1, -1, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_WRITE, "!a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_MOVE, "!a", "b", NULL);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_WRITE, "b", NULL, NULL);

    /* Shorten equality condition
     * PUSH <a>        => JUMP <l> <a> <b>
     * PUSH <b>
     * EQS
     * PUSH false
     * JUMPS <l>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_EQUAL_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, ">_", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL_STACK, "&l", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_JUMP_IF_NOT_EQUAL, "&l", "a", "b");

    /* Shorten non-equality condition
     * PUSH <a>        => JUMPNEQ <l> <a> <b>
     * PUSH <b>
     * EQS
     * NOTS
     * PUSH false
     * JUMPS <l>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_EQUAL_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_NOT_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, ">_", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL_STACK, "&l", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL, "&l", "a", "b");

    /* Shorten greater condition
     * PUSHS <a>      =>  GT temp1 <a> <b>
     * PUSHS <b>      =>  JUMPIFEQ <l> temp1 <false>
     * GTS
     * PUSH <false>
     * JUMPIFEQS <l>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_GREATER_THEN_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, ">f", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL_STACK, "&l", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_GREATER_THEN, "temp", "a", "b");
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL, "&l", "temp", ">f");

    /* Shorten lesser condition
     * PUSHS <a>      =>  LT temp1 <a> <b>
     * PUSHS <b>      =>  JUMPIFEQ <l> temp1 <false>
     * LTS
     * PUSH <false>
     * JUMPIFEQS <l>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_LESSER_THEN_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, ">f", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL_STACK, "&l", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_LESSER_THEN, "temp", "a", "b");
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL, "&l", "temp", ">f");

    /* Shorten lesser or equal condition
     * PUSHS <a>      =>  GT temp1 <a> <b>
     * PUSHS <b>      =>  JUMPIFEQ <l> temp1 <false>
     * GTS
     * NOTS
     * PUSH <false>
     * JUMPIFEQS <l>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_GREATER_THEN_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_NOT_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, ">f", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL_STACK, "&l", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_GREATER_THEN, "temp", "a", "b");
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_JUMP_IF_NOT_EQUAL, "&l", "temp", ">f");


    /* Shorten greater or equal condition
     * PUSHS <a>      =>  LT temp1 <a> <b>
     * PUSHS <b>      =>  JUMPIFEQ <l> temp1 <false>
     * LTS
     * NOTS
     * PUSH <false>
     * JUMPIFEQS <l>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_LESSER_THEN_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_NOT_STACK, NULL, NULL, NULL, 0, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, ">f", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL_STACK, "&l", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_LESSER_THEN, "temp", "a", "b");
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_JUMP_IF_NOT_EQUAL, "&l", "temp", ">f");

    /* Unreal jump.
     * PUSHS <t>        => E
     * PUSHS <f>
     * JUMPIFEQS <l>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "<", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, ">", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL_STACK, "&l", NULL, NULL, -1, 0, 0);

    /* Always jump.
     * PUSHS <t>        => JUMP <l>
     * PUSHS <t>
     * JUMPIFEQS <l>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP_IF_EQUAL_STACK, "&l", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_JUMP, "&l", NULL, NULL);

    /* Skip empty else branch.
     * JUMP <l>        => LABEL <k>
     * LABEL <k>
     * LABEL <l>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_JUMP, "%l", NULL, NULL, 1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_LABEL, "%k", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_LABEL, "&l", NULL, NULL, 1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_LABEL, "%k", NULL, NULL);

    /* Use move instead of stack if (b = a)
     * PUSH <a>         => MOVE <b> <a>
     * POP <b>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_MOVE, "!b", "a", NULL);

    /* Delete move which is overwritten with another move (a = b; a = c) => (a = c)
     * MOVE <a> <b>     => MOVE <a> <c>
     * MOVE <a> <c>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "!a", "b", NULL, -1, -1, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "!a", "c", NULL, -1, -1, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_MOVE, "!a", "c", NULL);

    // TODO doc
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "1t", "a", NULL, -1, -1, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_WRITE, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_WRITE, "a", NULL, NULL);


    /* Use move instead of stack if (b = a)
     * PUSH <a>         => MOVE <b> <a>
     * POP <b>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_PUSH_STACK, "a", NULL, NULL, -1, 0, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_POP_STACK, "!b", NULL, NULL, -1, 0, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_MOVE, "!b", "a", NULL);

    /* Delete move which is overwritten with another move (a = b; a = c) => (a = c)
     * MOVE <a> <b>     => MOVE <a> <c>
     * MOVE <a> <c>
     */
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "!a", "b", NULL, -1, -1, 0);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_MOVE, "!a", "c", NULL, -1, -1, 0);
    code_optimizer_add_replacement_instruction_to_ph_pattern(pattern, I_MOVE, "!a", "c", NULL);

}

void code_optimizer_free(CodeOptimizer** optimizer) {
    NULL_POINTER_CHECK(optimizer,);
    NULL_POINTER_CHECK(*optimizer,);

    oriented_graph_free(&(*optimizer)->code_graph);
    symbol_table_free((*optimizer)->variables_meta_data);
    symbol_table_free((*optimizer)->functions_meta_data);
    symbol_table_free((*optimizer)->labels_meta_data);
    interpreter_free(&(*optimizer)->interpreter);
    llist_free(&(*optimizer)->peep_hole_patterns);
    memory_free(*optimizer);
    *optimizer = NULL;
}

void code_optimizer_reset_variable_meta_data(const char* key, void* item, void* data) {
    NULL_POINTER_CHECK(item,);
    VariableMetaData* v = (VariableMetaData*) item;
    v->occurrences_count = 0;
    v->purity_type = META_TYPE_PURE;
    v->read_usage_count = 0;
}

void code_optimizer_reset_label_meta_data(const char* key, void* item, void* data) {
    NULL_POINTER_CHECK(item,);
    LabelMetaData* v = (LabelMetaData*) item;
    v->occurrences_count = 0;
}

void code_optimizer_reset_function_meta_data(const char* key, void* item, void* data) {
    NULL_POINTER_CHECK(item,);
    FunctionMetaData* v = (FunctionMetaData*) item;
    v->call_count = 0;
    v->purity_type = META_TYPE_PURE;
    symbol_table_clear_buckets(v->mod_global_vars);
    symbol_table_clear_buckets(v->read_global_vars);
}

void code_optimizer_update_meta_data(CodeOptimizer* optimizer) {
    NULL_POINTER_CHECK(optimizer,);

    // clear old statistics
    symbol_table_foreach(optimizer->variables_meta_data, code_optimizer_reset_variable_meta_data, NULL);
    symbol_table_foreach(optimizer->labels_meta_data, code_optimizer_reset_label_meta_data, NULL);
    symbol_table_foreach(optimizer->functions_meta_data, code_optimizer_reset_function_meta_data, NULL);

    CodeInstruction* instruction = optimizer->generator->first;
    const char* current_function = NULL;

    while(instruction != NULL) {
        if(instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_FUNCTION_START) {
            ASSERT(instruction->type == I_LABEL);
            current_function = instruction->op0->data.label;
        }

        if(instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_FUNCTION_END)
            current_function = NULL;

        code_optimizer_adding_instruction(optimizer, instruction);
        code_optimizer_update_function_meta_data(optimizer, instruction, current_function);

        instruction = instruction->next;
    }

    // another loop to spread dynamic and check literal expression
    instruction = optimizer->generator->first;
    CodeInstruction* expr_start_instruction = NULL;
    while(instruction != NULL) {
        if((instruction->meta_data.type & CODE_INSTRUCTION_META_TYPE_EXPRESSION_START) &&
           instruction->meta_data.purity_type == META_TYPE_PURE) {
            expr_start_instruction = instruction;
            // assume literal expression
            instruction->meta_data.interpretable = true;
        }
        if(instruction->meta_data.type & CODE_INSTRUCTION_META_TYPE_EXPRESSION_END)
            expr_start_instruction = NULL;

        // spread function purity
        if(expr_start_instruction != NULL && instruction->type == I_CALL) {
            FunctionMetaData* func_meta_data = code_optimizer_function_meta_data(optimizer,
                                                                                 instruction->op0->data.label);
            expr_start_instruction->meta_data.purity_type |= func_meta_data->purity_type;
        }

        if(expr_start_instruction != NULL && expr_start_instruction->meta_data.interpretable) {
            // check if it is interpretable
            if(!interpreter_supported_instruction(instruction->type)) {
                expr_start_instruction->meta_data.interpretable = false;
            } else {
                CodeInstructionOperand* operands[OPERANDS_MAX_COUNT] = {
                        instruction->op0,
                        instruction->op1,
                        instruction->op2
                };

                for(int j = 0; j < instruction->signature_buffer->operand_count; j++) {
                    if(operands[j] != NULL &&
                       operands[j]->type == TYPE_INSTRUCTION_OPERAND_VARIABLE &&
                       !symbol_variable_cmp(optimizer->temp1, operands[j]->data.variable)) {
                        expr_start_instruction->meta_data.interpretable = false;
                        break;
                    }
                }
            }
        }

        instruction = instruction->next;
    }
}

void code_optimizer_update_function_meta_data(CodeOptimizer* optimizer, CodeInstruction* instruction,
                                              const char* current_func_label) {
    NULL_POINTER_CHECK(optimizer,);
    NULL_POINTER_CHECK(instruction,);

    if(instruction->type == I_CALL) {
        FunctionMetaData* function_meta_data = code_optimizer_function_meta_data(optimizer,
                                                                                 instruction->op0->data.label);
        function_meta_data->call_count++;
    }

    if(current_func_label == NULL)
        return;

    MetaType flags = META_TYPE_PURE;
    if(instruction->type == I_READ)
        flags |= META_TYPE_DYNAMIC_DEPENDENT;
    else if(instruction->type == I_WRITE)
        flags |= META_TYPE_OUTPUTED;
    if((instruction_class(instruction) == INSTRUCTION_TYPE_WRITE ||
        instruction_class(instruction) == INSTRUCTION_TYPE_VAR_MODIFIERS) &&
       instruction->op0->type == TYPE_INSTRUCTION_OPERAND_VARIABLE &&
       instruction->op0->data.variable->frame == VARIABLE_FRAME_GLOBAL) {
        flags |= META_TYPE_WITH_SIDE_EFFECT;
    }
    FunctionMetaData* meta_data = code_optimizer_function_meta_data(optimizer, current_func_label);
    meta_data->purity_type |= flags;

    // set modified global variables
    const TypeInstructionClass instruction_cls = instruction_class(instruction);
    if(instruction_cls == INSTRUCTION_TYPE_WRITE ||
       instruction_cls == INSTRUCTION_TYPE_VAR_MODIFIERS) {
        SymbolVariable* variable = instruction->op0->data.variable;
        if(variable->frame == VARIABLE_FRAME_GLOBAL) {
            symbol_table_get_or_create(meta_data->mod_global_vars, variable_cached_identifier(variable));
        }
    }

    // set read global variables
    CodeInstructionOperand* operands[OPERANDS_MAX_COUNT] = {
            instruction->op0,
            instruction->op1,
            instruction->op2
    };

    const TypeInstructionOperand operands_type[OPERANDS_MAX_COUNT] = {
            instruction->signature_buffer->type0,
            instruction->signature_buffer->type1,
            instruction->signature_buffer->type2,
    };

    for(int j = 0; j < instruction->signature_buffer->operand_count; j++) {
        if(operands[j]->type == TYPE_INSTRUCTION_OPERAND_VARIABLE &&
           operands_type[j] == TYPE_INSTRUCTION_OPERAND_SYMBOL) {
            SymbolVariable* variable = operands[j]->data.variable;
            if(variable->frame == VARIABLE_FRAME_GLOBAL) {
                symbol_table_get_or_create(meta_data->read_global_vars, variable_cached_identifier(variable));
            }
        }
    }
}

void code_optimizer_update_label_meta_data(CodeOptimizer* optimizer, CodeInstruction* instruction) {
    NULL_POINTER_CHECK(optimizer,);
    NULL_POINTER_CHECK(instruction,);

    switch(instruction->type) {
        case I_CALL:
        case I_JUMP:
        case I_JUMP_IF_EQUAL:
        case I_JUMP_IF_NOT_EQUAL:
        case I_JUMP_IF_EQUAL_STACK:
        case I_JUMP_IF_NOT_EQUAL_STACK:
            code_optimizer_label_meta_data(optimizer, instruction->op0->data.label)->occurrences_count++;
            break;
        default:
            break;
    }
}

VariableMetaData* code_optimizer_variable_meta_data(CodeOptimizer* optimizer, SymbolVariable* variable) {
    NULL_POINTER_CHECK(optimizer, NULL);
    NULL_POINTER_CHECK(variable, NULL);

    if(variable->_cached_identifier == NULL)
        variable->_cached_identifier = code_instruction_render_variable_identifier(variable);

    VariableMetaData* var_meta_data = (VariableMetaData*) symbol_table_get_or_create(
            optimizer->variables_meta_data,
            variable->_cached_identifier
    );

    return var_meta_data;
}

FunctionMetaData* code_optimizer_function_meta_data(CodeOptimizer* optimizer, const char* key) {
    NULL_POINTER_CHECK(optimizer, NULL);

    return (FunctionMetaData*) symbol_table_get_or_create(optimizer->functions_meta_data, key);
}

LabelMetaData* code_optimizer_label_meta_data(CodeOptimizer* optimizer, const char* label) {
    NULL_POINTER_CHECK(optimizer, NULL);

    return (LabelMetaData*) symbol_table_get_or_create(optimizer->labels_meta_data, label);
}

bool code_optimizer_remove_unused_variables(CodeOptimizer* optimizer, bool hard_remove, bool remove_special_temp) {
    NULL_POINTER_CHECK(optimizer, false);

    CodeInstruction* instruction = optimizer->generator->first;

    const size_t max_operands_count = 3;
    bool remove_something = false;
    MetaType expression_purity = META_TYPE_PURE;

    while(instruction != NULL) {
        bool delete_instruction = false;
        bool delete_expression = false;

        const CodeInstructionOperand* operands[] = {
                instruction->op0, instruction->op1, instruction->op2
        };

        if(instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_START)
            expression_purity |= instruction->meta_data.purity_type;

        for(size_t i = 0; i < max_operands_count; i++) {
            if(operands[i] == NULL || operands[i]->type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
                continue;

            SymbolVariable* variable = operands[i]->data.variable;
            const int variable_occurrences_count = code_optimizer_variable_meta_data(
                    optimizer,
                    variable
            )->occurrences_count;
            if(variable_occurrences_count == 0 && variable->frame != VARIABLE_FRAME_TEMP) {
                delete_expression = instruction->type == I_POP_STACK &&
                                    instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_END &&
                                    expression_purity == META_TYPE_PURE;

                if(!remove_special_temp && symbol_variable_cmp(variable, optimizer->temp6)) {
                    delete_instruction = false;
                    delete_expression = false;
                    break;
                }
                delete_instruction = !delete_expression;
                if(!hard_remove) {
                    delete_instruction = true;
                    delete_expression = false;
                }
                remove_something = true;
                break;
            }
        }

        if(delete_instruction) {
            CodeInstruction* temp = instruction->next;
            code_optimizer_removing_instruction(optimizer, instruction);
            code_generator_remove_instruction(optimizer->generator, instruction);
            instruction = temp;
        } else if(delete_expression) {
            CodeInstruction* expr_instruction = instruction;
            CodeInstruction* prev_expr_instruction;

            while(expr_instruction->meta_data.type != CODE_INSTRUCTION_META_TYPE_EXPRESSION_START) {
                prev_expr_instruction = expr_instruction->prev;
                code_optimizer_removing_instruction(optimizer, expr_instruction);
                code_generator_remove_instruction(optimizer->generator, expr_instruction);
                expr_instruction = prev_expr_instruction;
            }

            instruction = expr_instruction->prev;
            code_optimizer_removing_instruction(optimizer, expr_instruction);
            code_generator_remove_instruction(optimizer->generator, expr_instruction);
        } else
            instruction = instruction->next;
    }

    return remove_something;
}

PeepHolePattern* code_optimizer_new_ph_pattern(CodeOptimizer* optimizer) {
    return (PeepHolePattern*) llist_new_tail_item(optimizer->peep_hole_patterns);
}

SymbolTable* code_optimizer_check_ph_pattern(CodeOptimizer* optimizer,
                                             PeepHolePattern* ph_pattern,
                                             CodeInstruction* instruction) {
    NULL_POINTER_CHECK(optimizer, NULL);
    NULL_POINTER_CHECK(ph_pattern, NULL);
    NULL_POINTER_CHECK(instruction, NULL);

    PeepHolePatternInstruction* pattern_instruction =
            (PeepHolePatternInstruction*) ph_pattern->matching_instructions->head;
    SymbolTable* mapped_operands = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(MappedOperand),
                                                     &init_mapped_operand_item,
                                                     &free_mapped_operand_item);
    MappedOperand* temp = (MappedOperand*) symbol_table_get_or_create(mapped_operands, "temp");
    temp->operand = code_instruction_operand_init_variable(optimizer->temp6);

    temp = (MappedOperand*) symbol_table_get_or_create(mapped_operands, "<true");
    temp->operand = code_instruction_operand_init_boolean(true);

    const int operands_max_count = 3;

    while(pattern_instruction != NULL) {
        // means end of program of instruction type mismatch
        if(instruction == NULL || pattern_instruction->type != instruction->type)
            goto patten_not_matched;

        // check operands
        const char* operands_aliases[] = {pattern_instruction->op0_alias, pattern_instruction->op1_alias,
                                          pattern_instruction->op2_alias};
        int operands_occ_count[] = {pattern_instruction->op0_occurrences_count,
                                    pattern_instruction->op1_occurrences_count,
                                    pattern_instruction->op2_occurrences_count};
        CodeInstructionOperand* operands[] = {instruction->op0, instruction->op1, instruction->op2};

        for(int i = 0; i < operands_max_count; i++) {
            if(operands_aliases[i] == NULL)
                continue;

            // check meta pattern flag type
            const MetaPHPatternFlag meta_type_flag = extract_flag(operands_aliases[i]);
            const bool meta_type_flag_matched = code_optimizer_check_operand_with_meta_type_flag(
                    optimizer,
                    operands[i],
                    meta_type_flag
            );

            if(!meta_type_flag_matched)
                goto patten_not_matched;

            MappedOperand* mapped_operand = (MappedOperand*) symbol_table_get_or_create(mapped_operands,
                                                                                        operands_aliases[i]);

            if(mapped_operand->operand == NULL)
                mapped_operand->operand = code_instruction_operand_copy(operands[i]);

            else {
                if(!code_instruction_operand_cmp(operands[i], mapped_operand->operand))
                    goto patten_not_matched;
            }

            // check occurrences count
            if(operands_occ_count[i] != -1) {
                if(mapped_operand->operand->type == TYPE_INSTRUCTION_OPERAND_VARIABLE) {
                    const VariableMetaData* var_meta_data = code_optimizer_variable_meta_data(optimizer,
                                                                                              mapped_operand->operand->data.variable);

                    if(var_meta_data->occurrences_count != operands_occ_count[i])
                        goto patten_not_matched;
                } else if(mapped_operand->operand->type == TYPE_INSTRUCTION_OPERAND_LABEL) {
                    const LabelMetaData* label_meta_data = code_optimizer_label_meta_data(optimizer,
                                                                                          mapped_operand->operand->data.label);
                    if(label_meta_data->occurrences_count != operands_occ_count[i])
                        goto patten_not_matched;
                }

            }
        }

        // set next pattern
        instruction = instruction->next;
        pattern_instruction = (PeepHolePatternInstruction*) pattern_instruction->base.next;
    }

    return mapped_operands;

    patten_not_matched:
    symbol_table_free(mapped_operands);
    return NULL;
}

bool code_optimizer_peep_hole_optimization(CodeOptimizer* optimizer) {
    NULL_POINTER_CHECK(optimizer, false);

    CodeInstruction* instruction = optimizer->generator->first;
    PeepHolePattern* pattern = NULL;
    bool removed_something = false;

    while(instruction != NULL) {
        bool removed_instruction = false;

        pattern = (PeepHolePattern*) optimizer->peep_hole_patterns->head;
        while(pattern != NULL) {
            SymbolTable* mapped_operands = code_optimizer_check_ph_pattern(optimizer, pattern, instruction);
            if(mapped_operands != NULL) {
                // add replacement
                PeepHolePatternInstruction* ph_pattern_instruction = (PeepHolePatternInstruction*) pattern->replacement_instructions->head;
                const size_t replacement_pattern_instruction_count = llist_length(pattern->replacement_instructions);
                for(size_t i = 0; i < replacement_pattern_instruction_count; i++) {
                    CodeInstruction* replacement_instruction = code_optimizer_new_instruction_with_mapped_operands(
                            optimizer, ph_pattern_instruction, mapped_operands);
                    code_generator_insert_instruction_before(optimizer->generator, replacement_instruction,
                                                             instruction);
                    ph_pattern_instruction = (PeepHolePatternInstruction*) ph_pattern_instruction->base.next;
                }

                // remove old
                const size_t matching_pattern_instruction_count = llist_length(pattern->matching_instructions);
                for(size_t i = 0; i < matching_pattern_instruction_count; i++) {
                    CodeInstruction* temp = instruction->next;
                    code_optimizer_removing_instruction(optimizer, instruction);
                    code_generator_remove_instruction(optimizer->generator, instruction);
                    instruction = temp;
                    removed_instruction = true;
                    removed_something = true;
                }

                symbol_table_free(mapped_operands);
                break;
            }
            pattern = (PeepHolePattern*) pattern->base.next;
        }

        if(!removed_instruction)
            instruction = instruction->next;
    }

    return removed_something;
}

CodeInstruction* code_optimizer_new_instruction_with_mapped_operands(CodeOptimizer* optimizer,
                                                                     PeepHolePatternInstruction* ph_pattern_instruction,
                                                                     SymbolTable* mapped_operands) {
    NULL_POINTER_CHECK(optimizer, NULL);
    NULL_POINTER_CHECK(ph_pattern_instruction, NULL);
    NULL_POINTER_CHECK(mapped_operands, NULL);

    const TypeInstruction instruction_type = ph_pattern_instruction->type;
    const short operands_count = code_generator_instruction_operands_count(optimizer->generator, instruction_type);
    const char* op_aliases[] = {ph_pattern_instruction->op0_alias, ph_pattern_instruction->op1_alias,
                                ph_pattern_instruction->op2_alias};
    CodeInstructionOperand* operands[] = {NULL, NULL, NULL};

    for(int i = 0; i < operands_count; i++) {
        if(op_aliases[i] != NULL) {
            operands[i] = code_instruction_operand_copy(
                    ((MappedOperand*) symbol_table_get_or_create(mapped_operands, op_aliases[i]))->operand);
        }
    }

    CodeInstruction* instruction = code_generator_new_instruction(
            optimizer->generator,
            ph_pattern_instruction->type,
            operands[0], operands[1], operands[2]
    );

    code_optimizer_adding_instruction(optimizer, instruction);

    return instruction;
}

bool code_optimizer_check_operand_with_meta_type_flag(CodeOptimizer* optimizer, CodeInstructionOperand* operand,
                                                      MetaPHPatternFlag meta_type_flag) {
    NULL_POINTER_CHECK(operand, NULL);

    if(meta_type_flag == META_PATTERN_FLAG_INVALID)
        return false;

    if(meta_type_flag != META_PATTERN_FLAG_ALL) {
        switch(meta_type_flag) {
            case META_PATTERN_FLAG_STRING:
            case META_PATTERN_FLAG_STRING_EMPTY:
                if(operand->type == TYPE_INSTRUCTION_OPERAND_CONSTANT &&
                   operand->data.constant.data_type == DATA_TYPE_STRING) {
                    if(meta_type_flag == META_PATTERN_FLAG_STRING)
                        return true;

                    const size_t len = string_length(operand->data.constant.data.string);
                    return len == 0 && meta_type_flag == META_PATTERN_FLAG_STRING_EMPTY;
                }
                return false;

            case META_PATTERN_FLAG_VARIABLE:
                return operand->type == TYPE_INSTRUCTION_OPERAND_VARIABLE;
            case META_PATTERN_FLAG_TEMP_VARIABLE_1:
            case META_PATTERN_FLAG_TEMP_VARIABLE_2:
            case META_PATTERN_FLAG_TEMP_VARIABLE_3:
            case META_PATTERN_FLAG_TEMP_VARIABLE_4:
            case META_PATTERN_FLAG_TEMP_VARIABLE_5: {
                if(operand->type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
                    return false;

                SymbolVariable* temp_var = NULL;
                if(meta_type_flag == META_PATTERN_FLAG_TEMP_VARIABLE_1)
                    temp_var = optimizer->temp1;
                else if(meta_type_flag == META_PATTERN_FLAG_TEMP_VARIABLE_2)
                    temp_var = optimizer->temp2;
                else if(meta_type_flag == META_PATTERN_FLAG_TEMP_VARIABLE_3)
                    temp_var = optimizer->temp3;
                else if(meta_type_flag == META_PATTERN_FLAG_TEMP_VARIABLE_4)
                    temp_var = optimizer->temp4;
                else if(meta_type_flag == META_PATTERN_FLAG_TEMP_VARIABLE_5)
                    temp_var = optimizer->temp5;

                const bool are_same = symbol_variable_cmp(temp_var, operand->data.variable);

                return are_same;
            }

            case META_PATTERN_FLAG_INT_LITERAL:
            case META_PATTERN_FLAG_INT_LITERAL_ZERO:
                if(operand->type == TYPE_INSTRUCTION_OPERAND_CONSTANT &&
                   operand->data.constant.data_type == DATA_TYPE_INTEGER) {
                    if(meta_type_flag == META_PATTERN_FLAG_INT_LITERAL)
                        return true;

                    const int ivalue = operand->data.constant.data.integer;
                    return ivalue == 0 && meta_type_flag == META_PATTERN_FLAG_INT_LITERAL_ZERO;
                }
                return false;

            case META_PATTERN_FLAG_FLOAT_LITERAL:
            case META_PATTERN_FLAG_FLOAT_LITERAL_ZERO:
                if(operand->type == TYPE_INSTRUCTION_OPERAND_CONSTANT &&
                   operand->data.constant.data_type == DATA_TYPE_DOUBLE) {
                    if(meta_type_flag == META_PATTERN_FLAG_FLOAT_LITERAL)
                        return true;

                    const double fvalue = operand->data.constant.data.double_;
                    return fvalue == 0. && meta_type_flag == META_PATTERN_FLAG_FLOAT_LITERAL_ZERO;
                }
                return false;

            case META_PATTERN_FLAG_BOOL_LITERAL:
            case META_PATTERN_FLAG_BOOL_LITERAL_TRUE:
            case META_PATTERN_FLAG_BOOL_LITERAL_FALSE:
                if(operand->type == TYPE_INSTRUCTION_OPERAND_CONSTANT &&
                   operand->data.constant.data_type == DATA_TYPE_BOOLEAN) {
                    if(meta_type_flag == META_PATTERN_FLAG_BOOL_LITERAL)
                        return true;

                    const bool bvalue = operand->data.constant.data.boolean;
                    return (bvalue && meta_type_flag == META_PATTERN_FLAG_BOOL_LITERAL_TRUE) ||
                           (!bvalue && meta_type_flag == META_PATTERN_FLAG_BOOL_LITERAL_FALSE);
                }
                return false;

            case META_PATTERN_FLAG_LABEL:
                return operand->type == TYPE_INSTRUCTION_OPERAND_LABEL;

            default:
                LOG_WARNING("Undefined meta pattern flag.");
                break;
        }
    }

    return true;
}

bool code_optimizer_remove_unused_functions(CodeOptimizer* optimizer) {
    NULL_POINTER_CHECK(optimizer, false);

    code_optimizer_update_meta_data(optimizer);

    CodeInstruction* instruction = optimizer->generator->first;
    bool removing_function = false;
    bool removed_something = false;

    while(instruction != NULL) {
        if(instruction->type == I_LABEL && instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_FUNCTION_START) {
            const FunctionMetaData* function_meta_data = code_optimizer_function_meta_data(optimizer,
                                                                                           instruction->op0->data.label);

            if(function_meta_data->call_count == 0)
                removing_function = true;
        }

        if(removing_function) {
            if(instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_FUNCTION_END)
                removing_function = false;
            CodeInstruction* next_instruction = instruction->next;
            code_generator_remove_instruction(optimizer->generator, instruction);
            removed_something = true;
            instruction = next_instruction;
        } else {
            instruction = instruction->next;
        }
    }
    return removed_something;
}

void code_optimizer_adding_instruction(CodeOptimizer* optimizer, CodeInstruction* instruction) {
    NULL_POINTER_CHECK(optimizer,);
    NULL_POINTER_CHECK(instruction,);

    const TypeInstruction instruction_type = instruction->type;
    if(instruction_type == I_DEF_VAR || instruction_type == I_POP_STACK)
        return;

    const short operands_count = code_generator_instruction_operands_count(optimizer->generator, instruction->type);
    const CodeInstructionOperand* operands[] = {instruction->op0, instruction->op1, instruction->op2};

    // handle label
    code_optimizer_update_label_meta_data(optimizer, instruction);

    for(int i = 0; i < operands_count; i++) {
        if(operands[i] == NULL) {
            LOG_WARNING("Operand is null when it shoul not.");
            return;
        }

        const TypeInstructionOperand operand_type = operands[i]->type;

        // update meta data

        if(operand_type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
            continue;

        if(instruction->type == I_MOVE && i == 0) // it's first operand
            continue;

        if(instruction_type == I_READ) {
            VariableMetaData* meta_data = code_optimizer_variable_meta_data(optimizer, operands[i]->data.variable);
            meta_data->occurrences_count++;
            meta_data->purity_type |= META_TYPE_DYNAMIC_DEPENDENT;
            meta_data->read_usage_count++;
            break;
        } else {
            VariableMetaData* meta_data = code_optimizer_variable_meta_data(optimizer, operands[i]->data.variable);
            meta_data->occurrences_count++;
        }
    }
}

void code_optimizer_removing_instruction(CodeOptimizer* optimizer, CodeInstruction* instruction) {
    NULL_POINTER_CHECK(optimizer,);
    NULL_POINTER_CHECK(instruction,);

    const TypeInstruction instruction_type = instruction->type;
    if(instruction_type == I_DEF_VAR || instruction_type == I_POP_STACK)
        return;

    // handle labels
    switch(instruction->type) {
        case I_CALL:
        case I_JUMP:
        case I_JUMP_IF_EQUAL:
        case I_JUMP_IF_NOT_EQUAL:
        case I_JUMP_IF_EQUAL_STACK:
        case I_JUMP_IF_NOT_EQUAL_STACK: {
            LabelMetaData* meta_data = code_optimizer_label_meta_data(optimizer, instruction->op0->data.label);
            if(meta_data->occurrences_count > 0)
                meta_data->occurrences_count--;
            return;
        }
        default:
            break;
    }

    const short operands_count = code_generator_instruction_operands_count(optimizer->generator, instruction->type);
    const CodeInstructionOperand* operands[] = {instruction->op0, instruction->op1, instruction->op2};

    for(int i = 0; i < operands_count; i++) {
        if(operands[i] == NULL) {
            LOG_WARNING("Operand is null when it shoul not.");
            return;
        }

        const TypeInstructionOperand operand_type = operands[i]->type;

        // update meta data

        if(operand_type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
            continue;

        if(instruction->type == I_MOVE && i == 0) // it's first operand
            continue;

        if(instruction_type == I_READ) {
            VariableMetaData* meta_data = code_optimizer_variable_meta_data(optimizer, operands[i]->data.variable);

            if(meta_data->occurrences_count > 0)
                meta_data->occurrences_count--;
            if(meta_data->read_usage_count > 0)
                meta_data->read_usage_count--;
            if(meta_data->read_usage_count == 0)
                meta_data->purity_type &= ~META_TYPE_DYNAMIC_DEPENDENT;
            break;
        } else {
            VariableMetaData* meta_data = code_optimizer_variable_meta_data(optimizer, operands[i]->data.variable);
            if(meta_data->occurrences_count > 0)
                meta_data->occurrences_count--;
        }
    }
}

void code_optimizer_split_code_to_graph(CodeOptimizer* optimizer) {
    NULL_POINTER_CHECK(optimizer,);

    oriented_graph_clear(optimizer->code_graph);

    CodeInstruction* instruction = optimizer->generator->first;
    CodeBlock* code_block = (CodeBlock*) oriented_graph_new_node(optimizer->code_graph);
    SymbolTable* mapped_blocks = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(SymbolTableIntItem), NULL, NULL);

    while(instruction != NULL) {
        const TypeInstructionClass prev_instruction_type_class = instruction_class(instruction->prev);
        const bool prev_is_direct_jump = (prev_instruction_type_class == INSTRUCTION_TYPE_DIRECT_JUMP);
        const bool prev_is_cond_jump = (prev_instruction_type_class == INSTRUCTION_TYPE_CONDITIONAL_JUMP);

        // creating new block
        if(instruction->type == I_LABEL || prev_is_cond_jump || prev_is_direct_jump) {
            CodeBlock* new_code_block = (CodeBlock*) oriented_graph_new_node(optimizer->code_graph);

            // map block
            if(instruction->type == I_LABEL) {
                SymbolTableIntItem* mapped_block = (SymbolTableIntItem*) symbol_table_get_or_create(mapped_blocks,
                                                                                                    instruction->op0->data.label);
                mapped_block->value = (int) new_code_block->base.id;
            }

            // can connect  indirect blocks
            if((prev_is_cond_jump || (instruction->type == I_LABEL && !prev_is_direct_jump)) &&
               code_block->last_instruction->type != I_RETURN) {
                oriented_graph_connect_nodes(
                        optimizer->code_graph,
                        (GraphNodeBase*) code_block,
                        (GraphNodeBase*) new_code_block);
                if(prev_is_cond_jump)
                    set_int_add(code_block->conditional_jump, (int) new_code_block->base.id);
            }
            code_block = new_code_block;
        }

        code_block_add_instruction(code_block, instruction);

        instruction = instruction->next;
    }

    // connect nodes
    OrientedGraph* graph = optimizer->code_graph;
    for(size_t i = 0; i < graph->capacity; i++) {
        if(graph->nodes[i] == NULL)
            continue;

        CodeBlock* block = (CodeBlock*) graph->nodes[i];
        if(block->last_instruction->type == I_RETURN)
            continue;
        const TypeInstructionClass last_block_instruction_type_class = instruction_class(block->last_instruction);
        if(last_block_instruction_type_class == INSTRUCTION_TYPE_DIRECT_JUMP ||
           last_block_instruction_type_class == INSTRUCTION_TYPE_CONDITIONAL_JUMP) {
            SymbolTableIntItem* mapped_block = (SymbolTableIntItem*) symbol_table_get_or_create(
                    mapped_blocks,
                    block->last_instruction->op0->data.label);

            if(mapped_block == NULL) {
                LOG_WARNING("Split code internal error");
                return;
            }

            // make connection
            if(last_block_instruction_type_class == INSTRUCTION_TYPE_CONDITIONAL_JUMP)
                set_int_add(block->conditional_jump, (unsigned int) mapped_block->value);
            oriented_graph_connect_nodes_by_ids(graph, block->base.id, (unsigned int) mapped_block->value);
        }
    }

    symbol_table_free(mapped_blocks);
}

bool code_optimizer_propate_constants_optimization(CodeOptimizer* optimizer) {
    NULL_POINTER_CHECK(optimizer, false);
    NULL_POINTER_CHECK(optimizer->code_graph, false);

    bool propagated_something = false;
    OrientedGraph* graph = optimizer->code_graph;
    LList* blocks_in_cycles = oriented_graph_scc(optimizer->code_graph);
    LList* cycled_blocks_mod_vars;
    llist_init(&cycled_blocks_mod_vars, sizeof(MetaDataCycledBlocksModVars), NULL,
               &meta_data_cycled_blocks_mod_vars_free, NULL);

    SetInt* proccessed_blocks = set_int_init();
    Stack* constants_tables_stack = stack_init(&constants_table_stack_item_free);

    // map modified variables in cycles
    LListItemSet* item = (LListItemSet*) blocks_in_cycles->head;
    while(item != NULL) {
        MetaDataCycledBlocksModVars* cycle_mod_vars = (MetaDataCycledBlocksModVars*) llist_new_tail_item(
                cycled_blocks_mod_vars);
        cycle_mod_vars->blocks_ids = set_int_copy(item->set);
        cycle_mod_vars->mod_vars = code_optimizer_modified_vars_in_blocks(optimizer, item->set);

        item = (LListItemSet*) item->base.next;
    }
    llist_free(&blocks_in_cycles);

    // propagate constants in functions
    for(unsigned int i = 0; i < graph->capacity; i++) {
        CodeBlock* block = (CodeBlock*) oriented_graph_node(graph, i);
        if(block == NULL)
            continue;

        if((block->instructions->meta_data.type & CODE_INSTRUCTION_META_TYPE_FUNCTION_START) == 0)
            continue;

        SymbolTable* constants = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(MappedOperand),
                                                   &init_mapped_operand_item,
                                                   &free_mapped_operand_item);
        constants->copy_data_callback = &copy_mapped_operand_item;
        stack_push(constants_tables_stack, (StackBaseItem*) constants_table_stack_item_init(constants));

        propagated_something |= code_optimizer_propagate_constants_in_block(optimizer, block, constants_tables_stack, proccessed_blocks,
                                                    cycled_blocks_mod_vars, false, false);
        StackBaseItem* old_table = stack_pop(constants_tables_stack);
        constants_table_stack_item_free(old_table);
        memory_free(old_table);
    }

    // propagate first block
    SymbolTable* constants = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(MappedOperand),
                                               &init_mapped_operand_item,
                                               &free_mapped_operand_item);
    constants->copy_data_callback = &copy_mapped_operand_item;
    stack_push(constants_tables_stack, (StackBaseItem*) constants_table_stack_item_init(constants));
    CodeBlock* block = (CodeBlock*) oriented_graph_node(graph, 0);

    // start
    propagated_something |= code_optimizer_propagate_constants_in_block(optimizer, block, constants_tables_stack, proccessed_blocks,
                                                cycled_blocks_mod_vars, false, true);

    llist_free(&cycled_blocks_mod_vars);
    stack_free(&constants_tables_stack);
    set_int_free(&proccessed_blocks);

    return propagated_something;
}

void block_variables_in_constants_table(const char* key, void* item, void* data) {
    NULL_POINTER_CHECK(item,);
    SymbolTable* constants_table = (SymbolTable*) data;
    MappedOperand* op = (MappedOperand*) symbol_table_function_get_or_create(constants_table, key);
    op->blocked = true;
    if(op->setter != NULL)
        op->setter->meta_data.without_effect = false;
}

void remove_variables_in_constants_table(const char* key, void* item, void* data) {
    NULL_POINTER_CHECK(item,);
    SymbolTable* constants_table = (SymbolTable*) data;
    MappedOperand* op = (MappedOperand*) symbol_table_function_get_or_create(constants_table, key);
    if(op->operand != NULL)
        code_instruction_operand_free(&op->operand);
}

// TODO rename
void remove_variables_setters_in_constants_table(const char* key, void* item, void* data) {
    NULL_POINTER_CHECK(item,);
    SymbolTable* constants_table = (SymbolTable*) data;
    MappedOperand* op = (MappedOperand*) symbol_table_function_get_or_create(constants_table, key);
    if(op->operand != NULL && op->setter != NULL)
        op->setter->meta_data.without_effect = false;
}

void remove_reset_var_setters_in_constants_table(const char* key, void* item, void* data) {
    NULL_POINTER_CHECK(item,);
    SymbolTable* constants_table = (SymbolTable*) data;
    MappedOperand* op = (MappedOperand*) symbol_table_function_get_or_create(constants_table, key);
    op->setter = NULL;
}

bool code_optimizer_propagate_constants_in_block(CodeOptimizer* optimizer,
                                                 CodeBlock* block,
                                                 Stack* constants_tables_stack,
                                                 SetInt* processed_blocks_ids,
                                                 LList* cycled_block_mod_vars,
                                                 bool is_conditional_block,
                                                 bool propagate_global_vars) {
    NULL_POINTER_CHECK(optimizer, false);
    NULL_POINTER_CHECK(block, false);
    NULL_POINTER_CHECK(constants_tables_stack, false);
    NULL_POINTER_CHECK(processed_blocks_ids, false);

    bool propagated_something = false;

    // check whether block was processed
    if(set_int_contains(processed_blocks_ids, (int) block->base.id))
        return false;
    set_int_add(processed_blocks_ids, (int) block->base.id);

    // get constants table
    SymbolTable* constants_table = NULL;
    // not copied lowest direct table
    SymbolTable* origin_constants_table = NULL;

    // determining direct parent blocks
    SetIntItem* parent_id = (SetIntItem*) block->base.in_edges->head;
    int direct_parent_blocks_count = 0;

    while(parent_id != NULL) {
        CodeBlock* parent_block = (CodeBlock*) oriented_graph_node(optimizer->code_graph,
                                                                   (unsigned int) parent_id->value);
        if(parent_block) {
            if(!set_int_contains(parent_block->conditional_jump, (int) block->base.id))
                direct_parent_blocks_count++;
        } else {
            LOG_WARNING("Internal error getting parent code block.");
        }

        parent_id = (SetIntItem*) parent_id->base.next;
    }

    // choose constants table
    if(!is_conditional_block && direct_parent_blocks_count <= 1)
        constants_table = ((ConstantsTableStackItem*) constants_tables_stack->head)->constants;
    else {
        constants_table = constants_tables_stack_get_lowest_direct_table(constants_tables_stack)->constants;
        origin_constants_table = constants_table;
    }
    constants_table = symbol_table_copy(constants_table);
    // reset table setters
    symbol_table_foreach(constants_table, &remove_reset_var_setters_in_constants_table, constants_table);

    // error
    if(constants_table == NULL) {
        LOG_WARNING("Internal error getting constants table.");
        return false;
    }

    // remove modified variables in cycles if block is in cycle
    MetaDataCycledBlocksModVars* cycled_blocks = (MetaDataCycledBlocksModVars*) cycled_block_mod_vars->head;
    while(cycled_blocks != NULL) {
        if(set_int_contains(cycled_blocks->blocks_ids, block->base.id)) {
            symbol_table_foreach(cycled_blocks->mod_vars, &block_variables_in_constants_table, constants_table);
        }

        cycled_blocks = (MetaDataCycledBlocksModVars*) cycled_blocks->base.next;
    }

    CodeInstruction* instruction = block->instructions;
    for(size_t i = 0; i < block->instructions_count; i++) {
        const TypeInstruction instruction_type = instruction->type;
        const TypeInstructionClass instruction_cls = instruction_class(instruction);

        // remove modified globals from function
        if(instruction->type == I_CALL) {
            SymbolTable* modified_globals = code_optimizer_function_meta_data(optimizer,
                                                                              instruction->op0->data.label)->mod_global_vars;
            if(modified_globals == NULL) {
                LOG_WARNING("Error getting function modified global vars table.");
            }

            symbol_table_foreach(modified_globals, &remove_variables_in_constants_table, constants_table);

            SymbolTable* read_globals = code_optimizer_function_meta_data(optimizer,
                                                                          instruction->op0->data.label)->read_global_vars;
            if(read_globals == NULL) {
                LOG_WARNING("Error getting function read global vars table.");
            }

            symbol_table_foreach(read_globals, &remove_variables_setters_in_constants_table, constants_table);
        }

        // replace variables with constant
        CodeInstructionOperand** operands[OPERANDS_MAX_COUNT] = {
                &instruction->op0,
                &instruction->op1,
                &instruction->op2
        };

        const TypeInstructionOperand operands_type[OPERANDS_MAX_COUNT] = {
                instruction->signature_buffer->type0,
                instruction->signature_buffer->type1,
                instruction->signature_buffer->type2,
        };

        for(int j = 0; j < OPERANDS_MAX_COUNT; j++) {
            if((*(operands[j])) != NULL &&
               (*(operands[j]))->type == TYPE_INSTRUCTION_OPERAND_VARIABLE &&
               operands_type[j] == TYPE_INSTRUCTION_OPERAND_SYMBOL) {
                SymbolVariable* variable = (*(operands[j]))->data.variable;
                MappedOperand* operand = (MappedOperand*) symbol_table_get(
                        constants_table, variable_cached_identifier(variable));

                // replace operand
                if(operand != NULL && operand->operand != NULL && !operand->blocked) {
                    propagated_something = true;
                    code_instruction_operand_free(&(*(operands[j])));
                    (*(operands[j])) = code_instruction_operand_copy(operand->operand);
                }
            }
        }

        // remove variable from constants table
        if(instruction_cls == INSTRUCTION_TYPE_WRITE ||
           instruction_cls == INSTRUCTION_TYPE_VAR_MODIFIERS) {
            SymbolVariable* variable = instruction->op0->data.variable;
            MappedOperand* operand = (MappedOperand*) symbol_table_get_or_create(
                    constants_table,
                    variable_cached_identifier(variable));

            MappedOperand* origin_table_operand = NULL;
            if(origin_constants_table != NULL) {
                origin_table_operand = (MappedOperand*) symbol_table_get_or_create(
                        origin_constants_table,
                        variable_cached_identifier(variable));
            }

            if(origin_table_operand != NULL && origin_table_operand->operand != NULL) {
                code_instruction_operand_free(&origin_table_operand->operand);
                origin_table_operand->setter = NULL;
            }
            if(operand->operand != NULL) {
                code_instruction_operand_free(&operand->operand);
                if(operand->setter != NULL &&
                   !operand->blocked &&
                   variable->frame != VARIABLE_FRAME_TEMP) {
                    operand->setter->meta_data.without_effect = true;
                }
                operand->setter = NULL;
            }

            // add variable to constants if second operand is constant
            if(!(!propagate_global_vars && variable->frame == VARIABLE_FRAME_GLOBAL)) {
                if(!operand->blocked && instruction_type == I_MOVE &&
                   instruction->op1->type == TYPE_INSTRUCTION_OPERAND_CONSTANT) {
                    // INFO new
                    operand->setter = instruction;
                    operand->operand = code_instruction_operand_copy(instruction->op1);
                }
            }
        }

        instruction = instruction->next;
    }

    // push table on stack
    ConstantsTableStackItem* stack_table = constants_table_stack_item_init(constants_table);
    stack_table->is_direct_table = !is_conditional_block;
    stack_push(constants_tables_stack, (StackBaseItem*) stack_table);

    SetInt* next_blocks_id = block->base.out_edges;
    SetIntItem* next_block_id = (SetIntItem*) next_blocks_id->head;
    while(next_block_id != NULL) {
        propagated_something |= code_optimizer_propagate_constants_in_block(
                optimizer,
                (CodeBlock*) oriented_graph_node(optimizer->code_graph, (unsigned int) next_block_id->value),
                constants_tables_stack,
                processed_blocks_ids,
                cycled_block_mod_vars,
                set_int_contains(block->conditional_jump, next_block_id->value),
                propagate_global_vars
        );
        next_block_id = (SetIntItem*) next_block_id->base.next;
    }

    ConstantsTableStackItem* old_constants_table = (ConstantsTableStackItem*) stack_pop(constants_tables_stack);
    constants_tables_stack->stack_item_free_callback((StackBaseItem*) old_constants_table);
    memory_free(old_constants_table);

    return propagated_something;
}

SymbolTable* code_optimizer_modified_vars_in_blocks(CodeOptimizer* optimizer, SetInt* blocks_ids) {
    NULL_POINTER_CHECK(optimizer, NULL);
    NULL_POINTER_CHECK(blocks_ids, NULL);

    SetIntItem* item = (SetIntItem*) blocks_ids->head;
    SymbolTable* mod_vars = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(SymbolTableBaseItem), NULL, NULL);

    while(item != NULL) {
        CodeBlock* block = (CodeBlock*) oriented_graph_node(optimizer->code_graph, (unsigned int) item->value);

        CodeInstruction* instruction = block->instructions;
        for(size_t i = 0; i < block->instructions_count; i++) {
            const TypeInstructionClass instruction_cls = instruction_class(instruction);
            if(instruction_cls == INSTRUCTION_TYPE_WRITE ||
               instruction_cls == INSTRUCTION_TYPE_VAR_MODIFIERS) {
                SymbolVariable* variable = instruction->op0->data.variable;
                symbol_table_get_or_create(
                        mod_vars, variable_cached_identifier(variable));
            }

            instruction = instruction->next;
        }

        item = (SetIntItem*) item->base.next;
    }

    return mod_vars;
}

bool code_optimizer_literal_expression_eval_optimization(CodeOptimizer* optimizer) {
    NULL_POINTER_CHECK(optimizer, false);

    CodeInstruction* instruction = optimizer->generator->first;
    CodeInstruction* start_instruction = NULL;
    bool interpreted_something = false;

    while(instruction != NULL) {
        if(instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_START &&
           instruction->meta_data.interpretable) {
            start_instruction = instruction;
        }

        if(instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_END &&
           start_instruction != NULL) {
            CodeInstruction* end_instruction = instruction;
            if(instruction->type == I_POP_STACK)
                end_instruction = instruction->prev;

            CodeInstructionOperand* lit_operand = interpreter_evaluate_instruction_block(
                    optimizer->interpreter,
                    start_instruction,
                    end_instruction);

            if(lit_operand != NULL) {
                CodeInstruction* new_instruction = NULL;
                interpreted_something = true;

                if(instruction->type == I_POP_STACK) {
                    new_instruction = code_generator_new_instruction(
                            optimizer->generator,
                            I_MOVE,
                            code_instruction_operand_copy(instruction->op0),
                            lit_operand,
                            NULL);
                } else {

                    new_instruction = code_generator_new_instruction(
                            optimizer->generator, I_PUSH_STACK, lit_operand, NULL, NULL);
                }

                code_optimizer_adding_instruction(optimizer, new_instruction);
                code_generator_insert_instruction_before(
                        optimizer->generator,
                        new_instruction,
                        instruction->next);

                // remove expression
                CodeInstruction* expr_instruction = instruction;
                CodeInstruction* prev_expr_instruction;

                while(expr_instruction->meta_data.type != CODE_INSTRUCTION_META_TYPE_EXPRESSION_START) {
                    prev_expr_instruction = expr_instruction->prev;
                    code_optimizer_removing_instruction(optimizer, expr_instruction);
                    code_generator_remove_instruction(optimizer->generator, expr_instruction);
                    expr_instruction = prev_expr_instruction;
                }

                instruction = expr_instruction->prev;
                code_optimizer_removing_instruction(optimizer, expr_instruction);
                code_generator_remove_instruction(optimizer->generator, expr_instruction);
            }
            start_instruction = NULL;
        }

        instruction = instruction->next;
    }

    return interpreted_something;
}

void code_optimizer_remove_instructions_without_effect_optimization(CodeOptimizer* optimizer) {
    NULL_POINTER_CHECK(optimizer,);

    CodeInstruction* instruction = optimizer->generator->first;
    CodeInstruction* next_instruction;
    while(instruction != NULL) {
        next_instruction = instruction->next;
        if(instruction->meta_data.without_effect) {
            code_optimizer_removing_instruction(optimizer, instruction);
            code_generator_remove_instruction(optimizer->generator, instruction);
        }

        instruction = next_instruction;
    }
}
