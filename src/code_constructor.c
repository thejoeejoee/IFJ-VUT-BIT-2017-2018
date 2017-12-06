#include "code_constructor.h"
#include "parser_expr_internal.h"


CodeConstructor* code_constructor_init() {
    CodeConstructor* constructor = memory_alloc(sizeof(CodeConstructor));

    constructor->generator = code_generator_init();
    constructor->scope_depth = 0;
    constructor->_label_counter = 0;
    constructor->control_statement_depth = 0;
    constructor->code_label_stack = stack_code_label_init();
    constructor->conditions_label_stack = stack_code_label_init();
    constructor->loops_label_stack = stack_code_label_init();
    constructor->loops_depth = 0;
    constructor->loops_initial_instruction = NULL;

    llist_init(&constructor->conversion_instructions, sizeof(TypeConversionInstruction), NULL, NULL, NULL);
    code_constructor_add_conversion_instruction(
            constructor,
            I_INT_TO_FLOAT,
            DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE,
            false
    );
    code_constructor_add_conversion_instruction(
            constructor,
            I_INT_TO_FLOAT_STACK,
            DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE,
            true
    );
    code_constructor_add_conversion_instruction(
            constructor,
            I_FLOAT_ROUND_TO_EVEN_INT,
            DATA_TYPE_DOUBLE,
            DATA_TYPE_INTEGER,
            false
    );
    code_constructor_add_conversion_instruction(
            constructor,
            I_FLOAT_ROUND_TO_EVEN_INT_STACK,
            DATA_TYPE_DOUBLE,
            DATA_TYPE_INTEGER,
            true
    );

    return constructor;
}

void code_constructor_free(CodeConstructor** constructor) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(*constructor,);

    stack_free(&(*constructor)->code_label_stack);
    stack_free(&(*constructor)->conditions_label_stack);
    stack_free(&(*constructor)->loops_label_stack);
    llist_free(&(*constructor)->conversion_instructions);
    code_generator_free(&((*constructor)->generator));
    memory_free(*constructor);
    *constructor = NULL;
}

void code_constructor_scope_start(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    constructor->scope_depth++;
}


void code_constructor_variable_declaration(CodeConstructor* constructor, SymbolVariable* symbol_variable) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(symbol_variable,);

    if(constructor->loops_initial_instruction == NULL) {
        // not in scope, directly declare variable
        GENERATE_CODE(
                I_DEF_VAR,
                code_instruction_operand_init_variable(symbol_variable)
        );
    } else {
        // in scope, insert declaration before loop
        CodeInstruction* declaration = code_generator_new_instruction(
                constructor->generator,
                I_DEF_VAR,
                code_instruction_operand_init_variable(symbol_variable),
                NULL,
                NULL
        );
        code_generator_insert_instruction_before(
                constructor->generator,
                declaration,
                constructor->loops_initial_instruction
        );

    }
    CodeInstructionOperand* operand = code_instruction_operand_implicit_value(symbol_variable->data_type);
    // variables not defined by user have not implicit value 
    if(operand != NULL)
        GENERATE_CODE(
                I_MOVE,
                code_instruction_operand_init_variable(symbol_variable),
                operand
        );
}


void code_constructor_shared_variable_declaration(CodeConstructor* constructor, SymbolVariable* symbol_variable) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(symbol_variable,);

    GENERATE_CODE(
            I_DEF_VAR,
            code_instruction_operand_init_variable(symbol_variable)
    );

    CodeInstructionOperand* operand = code_instruction_operand_implicit_value(symbol_variable->data_type);
    // variables not defined by user have not implicit value
    if(operand != NULL)
        GENERATE_CODE(
                I_MOVE,
                code_instruction_operand_init_variable(symbol_variable),
                operand
        );
}

void code_constructor_static_variable_declaration(CodeConstructor* constructor, SymbolVariable* static_variable,
                                                  SymbolFunction* function) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(static_variable,);
    // TODO: what about static in cycle?

    String* skip_label_string = string_init();
    string_append_s(skip_label_string, "DECLARED__");
    if(function != NULL)
        string_append_s(skip_label_string, function->base.key);
    string_append_s(skip_label_string, "_");
    string_append_s(skip_label_string, static_variable->base.key);
    string_append_s(skip_label_string, "_");

    char* scope_depth = memory_alloc(sizeof(char) * (64 + 1));
    snprintf(scope_depth, 64, "%lu", (unsigned long) static_variable->scope_depth);
    string_append_s(skip_label_string, scope_depth);
    memory_free(scope_depth);


    char* skip_label = code_constructor_generate_label(constructor, string_content(skip_label_string));
    stack_code_label_push(constructor->code_label_stack, skip_label);
    string_free(&skip_label_string);

    SymbolVariable* declaration_flag_variable = symbol_variable_init_flag_for_static_variable(
            static_variable,
            function
    );

    static_variable->frame = VARIABLE_FRAME_GLOBAL;

    // initialize to false (inverted order to respect stack-behaviour of inserting)
    CodeInstruction* declaration_flag_instruction_init = code_generator_new_instruction(
            constructor->generator,
            I_MOVE,
            code_instruction_operand_init_variable(declaration_flag_variable),
            code_instruction_operand_init_boolean(false),
            NULL
    );
    code_generator_insert_instruction_before(
            constructor->generator,
            declaration_flag_instruction_init,
            NULL
    );
    // declared flag declaration
    CodeInstruction* declaration_flag_instruction = code_generator_new_instruction(
            constructor->generator,
            I_DEF_VAR,
            code_instruction_operand_init_variable(declaration_flag_variable),
            NULL,
            NULL
    );
    code_generator_insert_instruction_before(
            constructor->generator,
            declaration_flag_instruction,
            NULL
    );

    // conditionally skip logic about initial value
    GENERATE_CODE(
            I_JUMP_IF_EQUAL,
            code_instruction_operand_init_label(stack_code_label_head(constructor->code_label_stack)),
            code_instruction_operand_init_boolean(true),
            code_instruction_operand_init_variable(declaration_flag_variable)
    );
    // declare and set implicit value
    GENERATE_CODE(
            I_DEF_VAR,
            code_instruction_operand_init_variable(static_variable)
    );
    GENERATE_CODE(
            I_MOVE,
            code_instruction_operand_init_variable(static_variable),
            code_instruction_operand_implicit_value(static_variable->data_type)
    );
    symbol_variable_single_free(&declaration_flag_variable);
}

void code_constructor_static_variable_declaration_end(
        CodeConstructor* constructor,
        SymbolVariable* static_variable,
        SymbolFunction* function
) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(static_variable,);

    CodeLabel* skip_label = stack_code_label_pop(constructor->code_label_stack);

    SymbolVariable* declaration_flag_variable = symbol_variable_init_flag_for_static_variable(
            static_variable,
            function
    );

    // set flag to initialized
    GENERATE_CODE(
            I_MOVE,
            code_instruction_operand_init_variable(declaration_flag_variable),
            code_instruction_operand_init_boolean(true)
    );
    // target label to skip declaration
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(skip_label->label)
    );

    code_label_free(&skip_label);
    symbol_variable_single_free(&declaration_flag_variable);
}

void code_constructor_input(CodeConstructor* constructor, SymbolVariable* symbol_variable) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(symbol_variable,);
    CHECK_VALID_DATA_TYPE(symbol_variable->data_type);

    String* prompt = string_init();
    string_append_s(prompt, "? ");

    GENERATE_CODE(
            I_WRITE,
            code_instruction_operand_init_string(prompt)
    );

    GENERATE_CODE(
            I_READ,
            code_instruction_operand_init_variable(symbol_variable),
            code_instruction_operand_init_data_type(symbol_variable->data_type)
    );
}

void code_constructor_if_after_expression(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    constructor->control_statement_depth++;
    // prepare end jump with label to first position in stack
    char* label = code_constructor_generate_label(constructor, "if_end");
    stack_code_label_push(constructor->conditions_label_stack, label);

    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(false)
    );

    label = code_constructor_generate_label(constructor, "if_else");
    GENERATE_CODE(
            I_JUMP_IF_EQUAL_STACK,
            code_instruction_operand_init_label(label)
    );
    stack_code_label_push(constructor->conditions_label_stack, label);
}

void code_constructor_if_end_if_block(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    char* label = stack_code_label_get_by_index(constructor->conditions_label_stack, 1);
    GENERATE_CODE(
            I_JUMP,
            code_instruction_operand_init_label(label)
    );
}


void code_constructor_if_after_end_if(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    CodeLabel* code_label = stack_code_label_pop(constructor->conditions_label_stack);
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(code_label->label)
    );
    // prepared endif
    code_label_free(&code_label);
    code_label = stack_code_label_pop(constructor->conditions_label_stack);
    NULL_POINTER_CHECK(code_label,);
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(code_label->label)
    );

    constructor->control_statement_depth--;
    code_label_free(&code_label);
}


void code_constructor_if_else_if_before_expression(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    char* label = stack_code_label_get_by_index(constructor->conditions_label_stack, 1);
    GENERATE_CODE(
            I_JUMP,
            code_instruction_operand_init_label(label)
    );

    CodeLabel* code_label = stack_code_label_pop(constructor->conditions_label_stack);
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(code_label->label)
    );

    code_label_free(&code_label);
}


void code_constructor_if_else_if_after_expression(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(false)
    );

    char* label = code_constructor_generate_label(constructor, "if_else");
    GENERATE_CODE(
            I_JUMP_IF_EQUAL_STACK,
            code_instruction_operand_init_label(label)
    );
    stack_code_label_push(constructor->conditions_label_stack, label);
}


void code_constructor_if_else_block(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    char* label = stack_code_label_get_by_index(constructor->conditions_label_stack, 1);
    GENERATE_CODE(
            I_JUMP,
            code_instruction_operand_init_label(label)
    );
    CodeLabel* code_label = stack_code_label_pop(constructor->conditions_label_stack);
    ASSERT(code_label != NULL);
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(code_label->label)
    );
    code_label_free(&code_label);
    label = code_constructor_generate_label(constructor, "if_else");
    stack_code_label_push(constructor->conditions_label_stack, label);
}


char* code_constructor_generate_label(CodeConstructor* constructor, const char* type) {
    NULL_POINTER_CHECK(constructor, NULL);
    NULL_POINTER_CHECK(type, NULL);

    const char* format = "%%%lu_%lu__%s";
    size_t len = strlen(type) + 2 * strlen(format);
    char* label = memory_alloc(len * sizeof(char));
    snprintf(
            label,
            len,
            format,
            (long unsigned) constructor->_label_counter++,
            (long unsigned) constructor->control_statement_depth,
            type
    );

    return label;
}

void code_constructor_print_expression(CodeConstructor* constructor, SymbolVariable* print_variable) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(print_variable,);

    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(print_variable)
    );

    GENERATE_CODE(
            I_WRITE,
            code_instruction_operand_init_variable(print_variable)
    );
}

void code_constructor_while_before_condition(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);
    constructor->control_statement_depth++;

    char* label = code_constructor_generate_label(constructor, "while_start");
    stack_code_label_push(constructor->loops_label_stack, label);

    CodeInstruction* loop_start_instruction = GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(label)
    );

    // push label instruction to declare variables before while loop
    if(constructor->loops_depth++ == 0) {
        constructor->loops_initial_instruction = loop_start_instruction;
    }
}


void code_constructor_while_after_condition(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(false)
    );

    char* label = code_constructor_generate_label(constructor, "while_end");
    GENERATE_CODE(
            I_JUMP_IF_EQUAL_STACK,
            code_instruction_operand_init_label(label)
    );
    stack_code_label_push(constructor->loops_label_stack, label);
}


void code_constructor_while_end(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    CodeLabel* end_label = stack_code_label_pop(constructor->loops_label_stack);
    CodeLabel* start_label = stack_code_label_pop(constructor->loops_label_stack);

    ASSERT(end_label != NULL);
    ASSERT(start_label != NULL);
    GENERATE_CODE(
            I_JUMP,
            code_instruction_operand_init_label(start_label->label)
    );

    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(end_label->label)
    );
    constructor->control_statement_depth--;

    code_label_free(&start_label);
    code_label_free(&end_label);

    // starting label instruction is now more needed
    if(--constructor->loops_depth == 0) {
        constructor->loops_initial_instruction = NULL;
    }
}

void code_constructor_variable_expression_assignment(CodeConstructor* constructor, SymbolVariable* variable) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(variable,);

    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(variable)
    );
}

void code_constructor_function_header(CodeConstructor* constructor, SymbolFunction* function) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(function,);

    constructor->generator->to_buffer = true;

    String* label = symbol_function_generate_function_label(function);
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(string_content(label))
    );
    string_free(&label);
}

void code_constructor_function_end(CodeConstructor* constructor, SymbolFunction* function) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(function,);

    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_implicit_value(function->return_data_type)
    );
    GENERATE_CODE(I_RETURN);

    constructor->generator->to_buffer = false;
}

void code_constructor_return(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    GENERATE_CODE(I_RETURN);
}

void code_constructor_scope_end(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    constructor->scope_depth--;
}

void code_constructor_stack_type_conversion(CodeConstructor* constructor, DataType current_type, DataType target_type) {
    NULL_POINTER_CHECK(constructor,);
    CHECK_VALID_DATA_TYPE(current_type);
    CHECK_VALID_DATA_TYPE(target_type);

    // TODO add bool in future
    TypeConversionInstruction* conversion_instruction = (TypeConversionInstruction*) constructor->conversion_instructions->head;

    while(conversion_instruction != NULL) {
        if(conversion_instruction->is_stack_instruction &&
           current_type == conversion_instruction->current_type &&
           target_type == conversion_instruction->target_type) {
            GENERATE_CODE(conversion_instruction->instruction);
            return;
        }

        conversion_instruction = (TypeConversionInstruction*) conversion_instruction->base.next;
    }

    LOG_WARNING("Undefined data type conversion for %d to %d.", current_type, target_type);
}

void code_constructor_add_conversion_instruction(CodeConstructor* constructor, TypeInstruction instruction,
                                                 DataType current_type, DataType target_type,
                                                 bool is_stack_instruction) {
    NULL_POINTER_CHECK(constructor,);
    CHECK_VALID_DATA_TYPE(current_type);
    CHECK_VALID_DATA_TYPE(target_type);

    TypeConversionInstruction* conversion_instruction = (TypeConversionInstruction*) llist_new_tail_item(
            constructor->conversion_instructions);

    conversion_instruction->instruction = instruction;
    conversion_instruction->current_type = current_type;
    conversion_instruction->target_type = target_type;
    conversion_instruction->is_stack_instruction = is_stack_instruction;
}

void code_constructor_binary_operation_stack_type_conversion(CodeConstructor* constructor, DataType operand_1_type,
                                                             DataType operand_2_type, DataType target_type,
                                                             SymbolVariable* temp_var) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(temp_var,);
    CHECK_VALID_DATA_TYPE(operand_1_type);
    CHECK_VALID_DATA_TYPE(operand_2_type);
    CHECK_VALID_DATA_TYPE(target_type);

    if(operand_1_type != target_type) {
        GENERATE_CODE(I_POP_STACK, code_instruction_operand_init_variable(temp_var));
        code_constructor_stack_type_conversion(constructor, operand_1_type, target_type);
        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_variable(temp_var));
    }

    if(operand_2_type != target_type) {
        code_constructor_stack_type_conversion(constructor, operand_2_type, target_type);
    }
}

void code_constructor_unary_operation_stack_type_conversion(CodeConstructor* constructor, DataType operand_type,
                                                            DataType target_type) {
    NULL_POINTER_CHECK(constructor,);
    CHECK_VALID_DATA_TYPE(operand_type);
    CHECK_VALID_DATA_TYPE(target_type);

    if(operand_type != target_type)
        code_constructor_stack_type_conversion(constructor, operand_type, target_type);
}

void code_constructor_fn_length(CodeConstructor* constructor, SymbolVariable* tmp_variable, DataType stack_param_type) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(tmp_variable,);

    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(stack_param_type, DATA_TYPE_STRING);
    GENERATE_CODE(I_POP_STACK, code_instruction_operand_init_variable(tmp_variable));
    GENERATE_CODE(
            I_STRING_LENGTH,
            code_instruction_operand_init_variable(tmp_variable),
            code_instruction_operand_init_variable(tmp_variable)
    );
    GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_variable(tmp_variable));
}


void code_constructor_fn_chr(CodeConstructor* constructor, SymbolVariable* tmp1, SymbolVariable* tmp2,
                             DataType param_type) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(tmp1,);
    NULL_POINTER_CHECK(tmp2,);

    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(param_type, DATA_TYPE_INTEGER);
    GENERATE_CODE(I_INT_TO_CHAR_STACK);
}

void code_constructor_fn_asc(
        CodeConstructor* constructor,
        SymbolVariable* tmp1,
        SymbolVariable* index,
        SymbolVariable* tmp3,
        DataType param_1_type,
        DataType param_2_type
) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(tmp1,);
    NULL_POINTER_CHECK(index,);
    NULL_POINTER_CHECK(tmp3,);

    char* zero_label = code_constructor_generate_label(constructor, "asc_zero");
    char* end_label = code_constructor_generate_label(constructor, "asc_end");

    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(param_2_type, DATA_TYPE_INTEGER);
    GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_integer(-1));
    GENERATE_CODE(I_ADD_STACK);
    GENERATE_CODE(I_POP_STACK, code_instruction_operand_init_variable(index));
    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(param_1_type, DATA_TYPE_STRING);
    GENERATE_CODE(I_POP_STACK, code_instruction_operand_init_variable(tmp1));
    GENERATE_CODE(
            I_STRING_LENGTH,
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_variable(tmp1)
    );
    GENERATE_CODE(
            I_LESSER_THEN,
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_variable(index),
            code_instruction_operand_init_variable(tmp3)
    );
    GENERATE_CODE(
            I_JUMP_IF_EQUAL,
            code_instruction_operand_init_label(zero_label),
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_boolean(false)
    );
    GENERATE_CODE(
            I_GREATER_THEN,
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_variable(index),
            code_instruction_operand_init_integer(-1)
    );
    GENERATE_CODE(
            I_JUMP_IF_EQUAL,
            code_instruction_operand_init_label(zero_label),
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_boolean(false)
    );
    GENERATE_CODE(
            I_STRING_TO_INT,
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_variable(tmp1),
            code_instruction_operand_init_variable(index)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_variable(tmp3)
    );
    GENERATE_CODE(
            I_JUMP,
            code_instruction_operand_init_label(end_label)
    );
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(zero_label)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(0)
    );
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(end_label)
    );
    memory_free(zero_label);
    memory_free(end_label);
}

void code_constructor_fn_substr(CodeConstructor* constructor, SymbolVariable* tmp1, SymbolVariable* tmp2,
                                SymbolVariable* tmp3, SymbolVariable* tmp4, SymbolVariable* tmp5,
                                ExprToken* string_expr,
                                ExprToken* index_expr, ExprToken* length_expr) {


    char* continue_label = code_constructor_generate_label(constructor, "substr_continue");
    char* loop_label = code_constructor_generate_label(constructor, "substr_loop");
    char* break_label = code_constructor_generate_label(constructor, "substr_break");
    char* empty_label = code_constructor_generate_label(constructor, "substr_empty");
    char* end_label = code_constructor_generate_label(constructor, "substr_end");

    String* empty_string = string_init();
    if((length_expr->is_constant || length_expr->is_variable) && length_expr->data_type == DATA_TYPE_INTEGER) {
        GENERATE_CODE(
                I_MOVE,
                code_instruction_operand_init_variable(tmp3),
                code_instruction_operand_copy(length_expr->instruction->op0)
        );
        code_generator_remove_instruction(constructor->generator, length_expr->instruction);
    } else {
        GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(length_expr->data_type, DATA_TYPE_INTEGER);
        GENERATE_CODE(
                I_POP_STACK,
                code_instruction_operand_init_variable(tmp3)
        );
    }
    if((index_expr->is_constant || index_expr->is_variable) && index_expr->data_type == DATA_TYPE_INTEGER) {
        GENERATE_CODE(
                I_MOVE,
                code_instruction_operand_init_variable(tmp2),
                code_instruction_operand_copy(index_expr->instruction->op0)
        );
        code_generator_remove_instruction(constructor->generator, index_expr->instruction);
    } else {
        GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(index_expr->data_type, DATA_TYPE_INTEGER);
        GENERATE_CODE(
                I_POP_STACK,
                code_instruction_operand_init_variable(tmp2)
        );
    }

    if((string_expr->is_constant || string_expr->is_variable) && string_expr->data_type == DATA_TYPE_STRING) {
        GENERATE_CODE(
                I_MOVE,
                code_instruction_operand_init_variable(tmp1),
                code_instruction_operand_copy(string_expr->instruction->op0)
        );
        code_generator_remove_instruction(constructor->generator, string_expr->instruction);
    } else {
        GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(string_expr->data_type, DATA_TYPE_STRING);
        GENERATE_CODE(
                I_POP_STACK,
                code_instruction_operand_init_variable(tmp1)
        );
    }

    GENERATE_CODE(
            I_LESSER_THEN,
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_variable(tmp2),
            code_instruction_operand_init_integer(1)
    );
    GENERATE_CODE(
            I_JUMP_IF_EQUAL,
            code_instruction_operand_init_label(empty_label),
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_boolean(true)
    );
    GENERATE_CODE(
            I_STRING_LENGTH,
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_variable(tmp1)
    );
    GENERATE_CODE(
            I_JUMP_IF_EQUAL,
            code_instruction_operand_init_label(empty_label),
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_integer(0)
    );

    GENERATE_CODE(
            I_SUB,
            code_instruction_operand_init_variable(tmp2),
            code_instruction_operand_init_variable(tmp2),
            code_instruction_operand_init_integer(1)
    );
    GENERATE_CODE(
            I_SUB,
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_variable(tmp2)
    );


    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_variable(tmp3)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(0)
    );
    GENERATE_CODE(
            I_LESSER_THEN_STACK
    );

    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_variable(tmp3)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_variable(tmp4)
    );
    GENERATE_CODE(I_GREATER_THEN_STACK);
    GENERATE_CODE(I_OR_STACK);
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(false)
    );
    GENERATE_CODE(
            I_JUMP_IF_EQUAL_STACK,
            code_instruction_operand_init_label(continue_label)
    );


    GENERATE_CODE(
            I_MOVE,
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_variable(tmp4)
    );
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(continue_label)
    );
    GENERATE_CODE(
            I_ADD,
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_variable(tmp2)
    );
    GENERATE_CODE(
            I_GREATER_THEN,
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_variable(tmp2),
            code_instruction_operand_init_variable(tmp3)
    );
    GENERATE_CODE(
            I_JUMP_IF_EQUAL,
            code_instruction_operand_init_label(empty_label),
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_boolean(true)
    );


    GENERATE_CODE(
            I_MOVE,
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_string(empty_string)
    );
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(loop_label)
    );
    GENERATE_CODE(
            I_JUMP_IF_EQUAL,
            code_instruction_operand_init_label(break_label),
            code_instruction_operand_init_variable(tmp2),
            code_instruction_operand_init_variable(tmp3)
    );

    GENERATE_CODE(
            I_GET_CHAR,
            code_instruction_operand_init_variable(tmp5),
            code_instruction_operand_init_variable(tmp1),
            code_instruction_operand_init_variable(tmp2)
    );
    GENERATE_CODE(
            I_CONCAT_STRING,
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_variable(tmp5)
    );

    GENERATE_CODE(
            I_ADD,
            code_instruction_operand_init_variable(tmp2),
            code_instruction_operand_init_variable(tmp2),
            code_instruction_operand_init_integer(1)
    );
    GENERATE_CODE(
            I_JUMP,
            code_instruction_operand_init_label(loop_label)
    );

    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(break_label)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_variable(tmp4)
    );
    GENERATE_CODE(
            I_JUMP,
            code_instruction_operand_init_label(end_label)
    );
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(empty_label)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_string(empty_string)
    );
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(end_label)
    );


    string_free(&empty_string);
    memory_free(break_label);
    memory_free(continue_label);
    memory_free(loop_label);
    memory_free(end_label);
    memory_free(empty_label);
}

void code_constructor_end_code(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    char* label = code_constructor_generate_label(constructor, "main_scope");
    GENERATE_CODE(
            I_JUMP,
            code_instruction_operand_init_label(label)
    );

    code_generator_flush_buffer(constructor->generator);

    GENERATE_CODE(I_LABEL, code_instruction_operand_init_label(label));
    memory_free(label);
}
