#ifndef _CODE_CONSTRUCTOR_H
#define _CODE_CONSTRUCTOR_H

#include "code_generator.h"
#include "stack_code_label.h"

#define GENERATE_1(name) code_generate_##name(constructor->generator)
#define GENERATE_2(name, op0) code_generate_##name(constructor->generator, op0)
#define GENERATE_3(name, op0, op1) code_generate_##name(constructor->generator, op0, op1)
#define GENERATE_4(name, op0, op1, op2) code_generate_##name(constructor->generator, op0, op1, op2)
#define GENERATE(...) MSVC_EXPAND(GET_OVERLOADED_MACRO1234(__VA_ARGS__, GENERATE_4, GENERATE_3, GENERATE_2, GENERATE_1)(__VA_ARGS__))

typedef struct code_constructor_t {
    // generator for raw target code
    CodeGenerator* generator;

    // metadata about actually generated code
    Stack* code_label_stack;
    size_t scope_depth;
    bool in_function_definition;
} CodeConstructor;

CodeConstructor* code_constructor_init();

void code_constructor_free(CodeConstructor** constructor);

void code_constructor_start_code(CodeConstructor* constructor);

void code_constructor_scope_start(CodeConstructor* constructor);

#endif //_CODE_CONSTRUCTOR_H
