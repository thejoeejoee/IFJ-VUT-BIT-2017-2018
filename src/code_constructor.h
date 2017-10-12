#ifndef _CODE_CONSTRUCTOR_H
#define _CODE_CONSTRUCTOR_H

#include "code_generator.h"
#include "stack_code_label.h"

#define _GENERATE_CODE_1(name) _GENERATE_CODE_4(name, NULL, NULL, NULL)
#define _GENERATE_CODE_2(name, op0) _GENERATE_CODE_4(name, op0, NULL, NULL)
#define _GENERATE_CODE_3(name, op0, op1) _GENERATE_CODE_4(name, op0, op1, NULL)
#define _GENERATE_CODE_4(name, op0, op1, op2) code_generator_instruction(constructor->generator, name, op0, op1, op2)
#define GENERATE_CODE(...) MSVC_EXPAND(GET_OVERLOADED_MACRO1234(__VA_ARGS__, _GENERATE_CODE_4, _GENERATE_CODE_3, \
    _GENERATE_CODE_2, _GENERATE_CODE_1)(__VA_ARGS__))

typedef struct code_constructor_t {
    // generator for raw target code
    CodeGenerator* generator;

    // metadata about actually generated code
    Stack* code_label_stack;
    size_t scope_depth;
    bool in_function_definition;

    size_t label_counter;
    size_t control_statement_depth;
} CodeConstructor;

CodeConstructor* code_constructor_init();

void code_constructor_free(CodeConstructor** constructor);

void code_constructor_start_code(CodeConstructor* constructor);

void code_constructor_scope_start(CodeConstructor* constructor);

void code_constructor_variable_declaration(CodeConstructor* constructor, int frame, SymbolVariable* symbol_variable);

void code_constructor_input(CodeConstructor* constructor, int frame, SymbolVariable* symbolVariable);

void code_constructor_if_after_expression(CodeConstructor* constructor);

void code_constructor_if_end_if_block(CodeConstructor* constructor);

void code_constructor_if_else_if_before_expression(CodeConstructor* constructor);

void code_constructor_if_else_if_after_expression(CodeConstructor* constructor);

void code_constructor_if_after_end_if(CodeConstructor* constructor);

void code_constructor_if_else_block(CodeConstructor* constructor);

void code_constructor_print_expression(CodeConstructor* constructor);

char* code_constructor_generate_label(CodeConstructor* constructor, const char* type, bool include_label_counter);


#endif //_CODE_CONSTRUCTOR_H
