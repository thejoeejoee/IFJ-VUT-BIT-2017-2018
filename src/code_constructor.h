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
    Stack* conditions_label_stack;
    Stack* loops_label_stack;
    size_t scope_depth;
    bool in_function_definition;

    size_t label_counter;
    size_t control_statement_depth;
} CodeConstructor;

CodeConstructor* code_constructor_init();

void code_constructor_free(CodeConstructor** constructor);

/**
 * Starts three address block with jump to main scope.
 * @param constructor
 */
void code_constructor_start_code(CodeConstructor* constructor);

/**
 * Generates target label for jump into main scope.
 * @param constructor
 */
void code_constructor_main_scope_start(CodeConstructor* constructor);

/**
 * Define variable in corresponding frame with correct data type.
 * @param constructor
 * @param frame
 * @param symbol_variable
 */
void code_constructor_variable_declaration(CodeConstructor* constructor, int frame, SymbolVariable* symbol_variable);

/**
 * Read from stdin into given variable.
 * @param constructor
 * @param frame
 * @param symbolVariable
 */
void code_constructor_input(CodeConstructor* constructor, int frame, SymbolVariable* symbolVariable);

/**
 * Generates control block to handle expression result and maybe skip positive statements.
 * @param constructor
 */
void code_constructor_if_after_expression(CodeConstructor* constructor);

/**
 * Jump from positive block out from whole if.
 * @param constructor
 */
void code_constructor_if_end_if_block(CodeConstructor* constructor);

/**
 * Generates target for negative if jump.
 * @param constructor
 */
void code_constructor_if_else_if_before_expression(CodeConstructor* constructor);

/**
 * Resolve elseif condition and maybe skip to next branch.
 * @param constructor
 */
void code_constructor_if_else_if_after_expression(CodeConstructor* constructor);

/**
 * Generates jump from positive branch to end of whole if.
 * @param constructor
 */
void code_constructor_if_after_end_if(CodeConstructor* constructor);

/**
 * Generates label for target, but without any next condition.
 * @param constructor
 */
void code_constructor_if_else_block(CodeConstructor* constructor);

/**
 * Generates target for jumps from end of while.
 * @param constructor
 */
void code_constructor_while_before_condition(CodeConstructor* constructor);

/**
 * Resolves condition and maybe skip the while block.
 * @param constructor
 */
void code_constructor_while_after_condition(CodeConstructor* constructor);

/**
 * Generates target for skipping whole while block.
 * @param constructor
 */
void code_constructor_while_end(CodeConstructor* constructor);

void code_constructor_print_expression(CodeConstructor* constructor);

char* code_constructor_generate_label(CodeConstructor* constructor, const char* type);

void code_constructor_variable_expression_assignment(CodeConstructor* constructor, SymbolVariable* variable);


#endif //_CODE_CONSTRUCTOR_H
