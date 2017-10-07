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

    code_generator_free(&((*constructor)->generator));
    memory_free(*constructor);
    *constructor = NULL;
}

void code_constructor_start_code(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    // TODO: random labels? hashed?
    stack_code_label_push(constructor->code_label_stack, "scope");
    GENERATE(I_JUMP, code_instruction_operand_init_label(stack_code_label_head(constructor->code_label_stack)));
}

void code_constructor_scope_start(CodeConstructor* constructor) {
    NULL_POINTER_CHECK(constructor,);

    if(constructor->in_function_definition) {
        // inc depth counter to declare variables with correct name in local frame
        constructor->scope_depth++;
    } else {
        // main program scope, generate label for jump from start of this file
        CodeLabel* scope_label = stack_code_label_pop(constructor->code_label_stack);
        ASSERT(scope_label == NULL);
        GENERATE(I_LABEL, code_instruction_operand_init_label(scope_label->label));
        GENERATE(
                I_JUMP_IF_EQUAL,
                code_instruction_operand_init_label("target"),
                code_instruction_operand_init_integer(42),
                code_instruction_operand_init_integer(34)
        );
    }
}

