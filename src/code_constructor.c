#include "code_constructor.h"

CodeConstructor* code_constructor_init() {
    CodeConstructor* constructor = memory_alloc(sizeof(CodeConstructor));

    constructor->generator = code_generator_init();
    constructor->scope_depth = 0;
    constructor->in_function_definition = false;
    constructor->label_counter = 0;
    constructor->control_statement_depth = 0;
    constructor->code_label_stack = stack_code_label_init();
    return constructor;
}

void code_constructor_free(CodeConstructor** constructor) {
    NULL_POINTER_CHECK(constructor,);
    NULL_POINTER_CHECK(*constructor,);

    stack_free(&(*constructor)->code_label_stack);
    code_generator_free(&((*constructor)->generator));
    memory_free(*constructor);
    *constructor = NULL;
}

void code_constructor_start_code(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    // TODO: random labels? hashed?
    stack_code_label_push(constructor->code_label_stack, "scope");
    GENERATE_CODE(I_JUMP, code_instruction_operand_init_label(stack_code_label_head(constructor->code_label_stack)));
}

void code_constructor_scope_start(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    if(constructor->in_function_definition) {
        // inc depth counter to declare variables with correct name in local frame
        constructor->scope_depth++;
    } else {
        // main program scope, generate label for jump from start of this file
        CodeLabel* scope_label = stack_code_label_pop(constructor->code_label_stack);
        ASSERT(scope_label != NULL);
        GENERATE_CODE(I_LABEL, code_instruction_operand_init_label(scope_label->label));
    }
}


void code_constructor_variable_declaration(CodeConstructor* constructor, int frame, SymbolVariable* symbol_variable) {
    NULL_POINTER_CHECK(constructor,);

    UNUSED(frame);

    // TODO: Add generationg symbol with corresponding frame
    // TODO: Add add inicialization on LF

    GENERATE_CODE(
            I_DEF_VAR,
            code_instruction_operand_init_variable(symbol_variable)
    );

    GENERATE_CODE(
            I_MOVE,
            code_instruction_operand_init_variable(symbol_variable),
            code_instruction_operand_init_integer(0)
    );
}

void code_constructor_input(CodeConstructor* constructor, int frame, SymbolVariable* symbol_variable) {
    NULL_POINTER_CHECK(constructor,);

    UNUSED(frame);

    // TODO: Add generationg symbol with corresponding frame
    // TODO: Add add inicialization on LF
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
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(false)
    );

    char* label = code_constructor_generate_label(constructor, "if_else", true);
    GENERATE_CODE(
            I_JUMP_IF_EQUAL_STACK,
            code_instruction_operand_init_label(label)
    );
    stack_code_label_push(constructor->code_label_stack, label);
}

void code_constructor_if_end_if_block(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);
    char* label = code_constructor_generate_label(constructor, "if_end", false);
    GENERATE_CODE(
            I_JUMP,
            code_instruction_operand_init_label(label)
    );
    memory_free(label);
}


void code_constructor_if_after_end_id(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);


    CodeLabel* code_label = stack_code_label_pop(constructor->code_label_stack);
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(code_label->label)
    );
    char* label = code_constructor_generate_label(constructor, "if_end", false);
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(label)
    );

    constructor->control_statement_depth--;
    memory_free(code_label->label);
    memory_free(code_label);
    memory_free(label);
}


void code_constructor_if_else_if_before_expression(CodeConstructor* constructor) {

    CodeLabel* code_label = stack_code_label_pop(constructor->code_label_stack);
    GENERATE_CODE(
            I_LABEL,
            code_instruction_operand_init_label(code_label->label)
    );
    memory_free(code_label->label);
    memory_free(code_label);
}


void code_constructor_if_else_if_after_expression(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(false)
    );

    char* label = code_constructor_generate_label(constructor, "if_end", true);
    GENERATE_CODE(
            I_JUMP_IF_EQUAL_STACK,
            code_instruction_operand_init_label(label)
    );
    stack_code_label_push(constructor->code_label_stack, label);
}


char* code_constructor_generate_label(CodeConstructor* constructor, const char* type, bool include_label_counter) {
    NULL_POINTER_CHECK(constructor, NULL);
    NULL_POINTER_CHECK(type, NULL);

    size_t len = strlen(type) + 16;
    char* label = memory_alloc(len * sizeof(char));
    if(include_label_counter) {
        snprintf(label, len, "_%05zd_%s_%05zd", constructor->label_counter++, type,
                 constructor->control_statement_depth);
    } else {
        snprintf(label, len, "_%s_%05zd", type, constructor->control_statement_depth);

    }

    return label;
}

void code_constructor_print_expression(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);
    // TODO: load expression
    GENERATE_CODE(
            I_WRITE,
            code_instruction_operand_init_integer(42)
    );
}
