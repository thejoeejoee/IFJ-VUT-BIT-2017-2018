#include "code_constructor.h"

CodeConstructor* code_constructor_init() {
    CodeConstructor* constructor = memory_alloc(sizeof(CodeConstructor));

    constructor->generator = code_generator_init();
    constructor->scope_depth = 0;
    constructor->in_function_definition = false;
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


    // TODO: Produce warning WARNING [code_instruction_operand.c:122:code_instruction_operand_render()]: Unknown data type to render: 257.
    GENERATE_CODE(
            I_READ,
            code_instruction_operand_init_variable(symbol_variable),
            code_instruction_operand_init_data_type(symbol_variable->data_type)
    );


}

