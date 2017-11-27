#include "code_optimizer.h"
#include "memory.h"

CodeOptimizer*
code_optimizer_init(CodeGenerator* generator, SymbolVariable* temp1, SymbolVariable* temp2, SymbolVariable* temp3,
                    SymbolVariable* temp4, SymbolVariable* temp5, SymbolVariable* temp6) {
    NULL_POINTER_CHECK(generator, NULL);
    NULL_POINTER_CHECK(temp1, NULL);
    NULL_POINTER_CHECK(temp2, NULL);
    NULL_POINTER_CHECK(temp3, NULL);
    NULL_POINTER_CHECK(temp4, NULL);
    NULL_POINTER_CHECK(temp5, NULL);
    NULL_POINTER_CHECK(temp6, NULL);


    CodeOptimizer* optimizer = memory_alloc(sizeof(CodeOptimizer));
    optimizer->variables_meta_data = symbol_table_init(32, sizeof(VariableMetaData), &init_variable_meta_data, NULL);

    optimizer->functions_meta_data = symbol_table_init(32, sizeof(FunctionMetaData), &init_function_meta_data, NULL);

    optimizer->labels_meta_data = symbol_table_init(32, sizeof(LabelMetaData), &init_label_meta_data, NULL);

    optimizer->generator = generator;
    optimizer->temp1 = temp1;
    optimizer->temp2 = temp2;
    optimizer->temp3 = temp3;
    optimizer->temp4 = temp4;
    optimizer->temp5 = temp5;
    optimizer->temp6 = temp6;

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

    /* Use normal operations with memory instead of stack if there is only 2 operands and result
     * is then stored in variable.
     * PUSH <a>         => OP <b> <a> <c>
     * PUSH <c>
     * OPS
     * POP <b>
     */
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
    pattern = code_optimizer_new_ph_pattern(optimizer);
    code_optimizer_add_matching_instruction_to_ph_pattern(pattern, I_LABEL, "&l", NULL, NULL, 0, 0, 0);

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


    return optimizer;
}

void code_optimizer_free(CodeOptimizer** optimizer) {
    NULL_POINTER_CHECK(optimizer,);
    NULL_POINTER_CHECK(*optimizer,);

    symbol_table_free((*optimizer)->variables_meta_data);
    symbol_table_free((*optimizer)->functions_meta_data);
    symbol_table_free((*optimizer)->labels_meta_data);
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

    // another loop to spread dynamic
    instruction = optimizer->generator->first;
    CodeInstruction* expr_start_instruction = NULL;
    while(instruction != NULL) {
        if(instruction->meta_data.type & CODE_INSTRUCTION_META_TYPE_EXPRESSION_START)
            expr_start_instruction = instruction;
        if(instruction->meta_data.type & CODE_INSTRUCTION_META_TYPE_EXPRESSION_END)
            expr_start_instruction = NULL;

        if(expr_start_instruction != NULL) {
            if(instruction->type == I_CALL) {
                FunctionMetaData* func_meta_data = code_optimizer_function_meta_data(optimizer,
                                                                                     instruction->op0->data.label);
                expr_start_instruction->meta_data.purity_type |= func_meta_data->purity_type;
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
    else {
        if((instruction->type == I_READ ||
            instruction->type == I_MOVE ||
            instruction->type == I_POP_STACK) &&
           instruction->op0->type == TYPE_INSTRUCTION_OPERAND_VARIABLE &&
           instruction->op0->data.variable->frame == VARIABLE_FRAME_GLOBAL)
            flags |= META_TYPE_WITH_SIDE_EFFECT;
    }
    code_optimizer_function_meta_data(optimizer, current_func_label)->purity_type |= flags;
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

MetaPHPatternFlag extract_flag(const char* alias) {
    if(alias == NULL || strlen(alias) == 0)
        return META_PATTERN_FLAG_INVALID;

    switch(alias[0]) {
        case ']':
            return META_PATTERN_FLAG_STRING_EMPTY;
        case '[':
            return META_PATTERN_FLAG_STRING;
        case '!':
            return META_PATTERN_FLAG_VARIABLE;
        case '}':
            return META_PATTERN_FLAG_INT_LITERAL_ZERO;
        case '{':
            return META_PATTERN_FLAG_INT_LITERAL;
        case ')':
            return META_PATTERN_FLAG_FLOAT_LITERAL_ZERO;
        case '(':
            return META_PATTERN_FLAG_FLOAT_LITERAL;
        case '|':
            return META_PATTERN_FLAG_BOOL_LITERAL;
        case '<':
            return META_PATTERN_FLAG_BOOL_LITERAL_TRUE;
        case '>':
            return META_PATTERN_FLAG_BOOL_LITERAL_FALSE;
        case '&':
            return META_PATTERN_FLAG_LABEL;
        case '1':
            return META_PATTERN_FLAG_TEMP_VARIABLE_1;
        case '2':
            return META_PATTERN_FLAG_TEMP_VARIABLE_2;
        case '3':
            return META_PATTERN_FLAG_TEMP_VARIABLE_3;
        case '4':
            return META_PATTERN_FLAG_TEMP_VARIABLE_4;
        case '5':
            return META_PATTERN_FLAG_TEMP_VARIABLE_5;
        default:
            return META_PATTERN_FLAG_ALL;
    }
}

void init_variable_meta_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    VariableMetaData* v = (VariableMetaData*) item;
    v->occurrences_count = 0;
    v->purity_type = META_TYPE_PURE;
    v->read_usage_count = 0;
}


void init_function_meta_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    FunctionMetaData* v = (FunctionMetaData*) item;
    v->call_count = 0;
    v->purity_type = META_TYPE_PURE;
}

void init_label_meta_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    LabelMetaData* v = (LabelMetaData*) item;
    v->occurrences_count = 0;
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

    // optimize
    instruction = optimizer->generator->first;

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
                if(!hard_remove)
                    delete_instruction = true;
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
            code_generator_remove_instruction(optimizer->generator, expr_instruction);
        } else
            instruction = instruction->next;
    }

    return remove_something;
}

void init_peep_hole_pattern(LListBaseItem* item) {
    PeepHolePattern* v = (PeepHolePattern*) item;
    llist_init(&v->matching_instructions, sizeof(PeepHolePatternInstruction), NULL, NULL, NULL);
    llist_init(&v->replacement_instructions, sizeof(PeepHolePatternInstruction), NULL, NULL, NULL);
}

void free_peep_hole_pattern(LListBaseItem* item) {
    PeepHolePattern* v = (PeepHolePattern*) item;
    llist_free(&v->matching_instructions);
    llist_free(&v->replacement_instructions);
}

PeepHolePattern* code_optimizer_new_ph_pattern(CodeOptimizer* optimizer) {
    return (PeepHolePattern*) llist_new_tail_item(optimizer->peep_hole_patterns);
}

void code_optimizer_add_matching_instruction_to_ph_pattern(PeepHolePattern* ph_pattern, TypeInstruction instruction,
                                                           const char* op1_alias, const char* op2_alias,
                                                           const char* op3_alias, int op0_occ_count, int op1_occ_count,
                                                           int op2_occ_count) {
    _code_optimizer_add_instruction_to_ph_pattern(ph_pattern->matching_instructions, instruction, op1_alias, op2_alias,
                                                  op3_alias, op0_occ_count, op1_occ_count, op2_occ_count);
}

void code_optimizer_add_replacement_instruction_to_ph_pattern(PeepHolePattern* ph_pattern, TypeInstruction instruction,
                                                              const char* op1_alias, const char* op2_alias,
                                                              const char* op3_alias) {
    _code_optimizer_add_instruction_to_ph_pattern(ph_pattern->replacement_instructions, instruction, op1_alias,
                                                  op2_alias, op3_alias, 0, 0, 0);
}

void _code_optimizer_add_instruction_to_ph_pattern(LList* pattern_instruction_sub_list, TypeInstruction instruction,
                                                   const char* op1_alias, const char* op2_alias, const char* op3_alias,
                                                   int op0_occ_count, int op1_occ_count, int op2_occ_count) {
    NULL_POINTER_CHECK(pattern_instruction_sub_list,);

    PeepHolePatternInstruction* ph_pattern_instruction = (PeepHolePatternInstruction*) llist_new_tail_item(
            pattern_instruction_sub_list
    );
    ph_pattern_instruction->type = instruction;
    ph_pattern_instruction->op0_alias = op1_alias;
    ph_pattern_instruction->op1_alias = op2_alias;
    ph_pattern_instruction->op2_alias = op3_alias;

    ph_pattern_instruction->op0_occurrences_count = op0_occ_count;
    ph_pattern_instruction->op1_occurrences_count = op1_occ_count;
    ph_pattern_instruction->op2_occurrences_count = op2_occ_count;
}

SymbolTable* code_optimizer_check_ph_pattern(CodeOptimizer* optimizer,
                                             PeepHolePattern* ph_pattern,
                                             CodeInstruction* instruction) {
    NULL_POINTER_CHECK(optimizer, NULL);
    NULL_POINTER_CHECK(ph_pattern, NULL);
    NULL_POINTER_CHECK(instruction, NULL);

//    code_optimizer_update_meta_data(optimizer);

    PeepHolePatternInstruction* pattern_instruction =
            (PeepHolePatternInstruction*) ph_pattern->matching_instructions->head;
    SymbolTable* mapped_operands = symbol_table_init(32, sizeof(MappedOperand), &init_mapped_operand_item,
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

void init_mapped_operand_item(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    MappedOperand* v = (MappedOperand*) item;
    v->operand = NULL;
}

void free_mapped_operand_item(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    MappedOperand* v = (MappedOperand*) item;
    if(v->operand != NULL)
        code_instruction_operand_free(&v->operand);
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
