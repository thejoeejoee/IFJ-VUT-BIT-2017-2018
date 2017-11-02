#include "code_constructor.h"


CodeConstructor* code_constructor_init() {
    CodeConstructor* constructor = memory_alloc(sizeof(CodeConstructor));

    constructor->generator = code_generator_init();
    constructor->scope_depth = 0;
    constructor->in_function_definition = false;
    constructor->label_counter = 0;
    constructor->control_statement_depth = 0;
    constructor->code_label_stack = stack_code_label_init();
    constructor->conditions_label_stack = stack_code_label_init();
    constructor->loops_label_stack = stack_code_label_init();

    llist_init(&constructor->conversion_instructions, sizeof(TypeConversionInstruction), NULL, NULL, NULL);

    code_constructor_add_conversion_instruction(constructor, I_INT_TO_FLOAT, DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE,
                                                false);
    code_constructor_add_conversion_instruction(constructor, I_INT_TO_FLOAT_STACK, DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE,
                                                true);

    code_constructor_add_conversion_instruction(constructor, I_FLOAT_ROUND_TO_EVEN_INT, DATA_TYPE_DOUBLE,
                                                DATA_TYPE_INTEGER,
                                                false);
    code_constructor_add_conversion_instruction(constructor, I_FLOAT_ROUND_TO_EVEN_INT_STACK, DATA_TYPE_DOUBLE,
                                                DATA_TYPE_INTEGER,
                                                true);
    // TODO add more data type conversions

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

void code_constructor_start_code(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    char* label = code_constructor_generate_label(constructor, "main_scope");
    stack_code_label_push(constructor->code_label_stack, label);
    GENERATE_CODE(I_JUMP, code_instruction_operand_init_label(stack_code_label_head(constructor->code_label_stack)));
}

void code_constructor_scope_start(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    if(constructor->scope_depth == 0) {
        // main program scope, generate label for jump from start of this file
        CodeLabel* scope_label = stack_code_label_pop(constructor->code_label_stack);
        ASSERT(scope_label != NULL);
        GENERATE_CODE(I_LABEL, code_instruction_operand_init_label(scope_label->label));
        GENERATE_CODE(I_CREATE_FRAME);
        GENERATE_CODE(I_PUSH_FRAME);
        code_label_free(&scope_label);
    }

    constructor->scope_depth++;
}


void code_constructor_variable_declaration(CodeConstructor* constructor, SymbolVariable* symbol_variable) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(symbol_variable,);

    // TODO: Add generating symbol with corresponding frame
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

    size_t len = strlen(type) + 16 + 16;
    char* label = memory_alloc(len * sizeof(char));
    snprintf(
            label,
            len,
            "%%_LABEL_%03zd_%s_DEPTH_%03zd",
            constructor->label_counter++,
            type,
            constructor->control_statement_depth
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

    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(label)
    );
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
}

void code_constructor_variable_expression_assignment(CodeConstructor* constructor, SymbolVariable* variable) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(variable,);

    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(variable)
    );
}

void code_constructor_generate_builtin_functions(CodeConstructor* constructor) {
    UNUSED(constructor);
    // TODO: Add generate code for built-in functions
}

void code_constructor_function_header(CodeConstructor* constructor, SymbolFunction* function) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(function,);

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
}

void code_constructor_return(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    GENERATE_CODE(I_RETURN);
}

void code_constructor_scope_end(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    constructor->scope_depth--;
    if(constructor->scope_depth == 0) {
        GENERATE_CODE(I_POP_FRAME);
    }
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

void code_constructor_fn_length(CodeConstructor* constructor, SymbolVariable* tmp_variable) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(tmp_variable,);

    GENERATE_CODE(I_POP_STACK, code_instruction_operand_init_variable(tmp_variable));
    GENERATE_CODE(
            I_STRING_LENGTH,
            code_instruction_operand_init_variable(tmp_variable),
            code_instruction_operand_init_variable(tmp_variable)
    );
    GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_variable(tmp_variable));
}


void code_constructor_fn_chr(CodeConstructor* constructor, SymbolVariable* tmp_variable) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(tmp_variable,);

    GENERATE_CODE(I_INT_TO_CHAR_STACK);
}

void code_constructor_fn_asc(
        CodeConstructor* constructor,
        SymbolVariable* tmp1,
        SymbolVariable* index,
        SymbolVariable* tmp3
) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(tmp1,);
    // NULL_POINTER_CHECK(index,);
    NULL_POINTER_CHECK(tmp3,);

    char* zero_label = code_constructor_generate_label(constructor, "asc_zero");
    char* end_label = code_constructor_generate_label(constructor, "asc_end");

    GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_integer(-1));
    GENERATE_CODE(I_ADD_STACK);
    GENERATE_CODE(I_POP_STACK, code_instruction_operand_init_variable(index));
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
                                SymbolVariable* tmp3, SymbolVariable* tmp4, SymbolVariable* tmp5) {


    char* continue_label = code_constructor_generate_label(constructor, "substr_continue");
    char* loop_label = code_constructor_generate_label(constructor, "substr_loop");
    char* break_label = code_constructor_generate_label(constructor, "substr_break");
    char* empty_label = code_constructor_generate_label(constructor, "substr_empty");
    char* end_label = code_constructor_generate_label(constructor, "substr_end");

    String* empty_string = string_init();


    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(tmp3)
    );


    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(tmp2)
    );
    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(tmp1)
    );

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
            I_PUSH_STACK,
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
            code_instruction_operand_init_variable(tmp3),
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_variable(tmp5)
    );
    GENERATE_CODE(
            I_MOVE,
            code_instruction_operand_init_variable(tmp4),
            code_instruction_operand_init_variable(tmp3)
    );
    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(tmp3)
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
