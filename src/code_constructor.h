#ifndef _CODE_CONSTRUCTOR_H
#define _CODE_CONSTRUCTOR_H

#include "code_generator.h"
#include "stack_code_label.h"

#define _GENERATE_CODE_1(name) _GENERATE_CODE_4(name, NULL, NULL, NULL)
#define _GENERATE_CODE_2(name, op0) _GENERATE_CODE_4(name, op0, NULL, NULL)
#define _GENERATE_CODE_3(name, op0, op1) _GENERATE_CODE_4(name, op0, op1, NULL)
#define _GENERATE_CODE_4(name, op0, op1, op2) code_generator_instruction(constructor->generator, name, op0, op1, op2)
#define GENERATE(...) MSVC_EXPAND(GET_OVERLOADED_MACRO1234(__VA_ARGS__, _GENERATE_CODE_4, _GENERATE_CODE_3, \
    _GENERATE_CODE_2, _GENERATE_CODE_1)(__VA_ARGS__))

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
