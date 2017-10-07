#ifndef _PARSER_SEMANTIC_H
#define _PARSER_SEMANTIC_H

#include "symbol_register.h"
#include "token.h"
#include "error.h"

typedef enum {
    ACTUAL_ACTION__NO_ACTION,
    ACTUAL_ACTION__FUNCTION_DECLARATION,
    ACTUAL_ACTION__FUNCTION_DEFINITION
} ActualAction;

typedef struct parser_semantic_t {
    SymbolRegister* register_;

    ErrorReport error_report;

    ActualAction actual_action;

    SymbolFunction* symbol_function; // Pointer to actual function

} ParserSemantic;

/**
 * @brief Set actual action for parser semantic. Sematic actions are scenarios. We have to keep the action.
 *
 * @param parser_semantic
 * @param actual_action
 */
void parser_semantic_set_action(ParserSemantic* parser_semantic, ActualAction actual_action);

/**
 * @brief Sets the function name for the current scenario.
 *
 * @param parserSemantic
 * @param char* name Name of function
 * @return bool
 */
bool parser_semantic_set_function_name(ParserSemantic* parserSemantic, char* name);

/**
 * Constructor for parser_semanatic
 *
 * @return ParserSematic*
 */
ParserSemantic* parser_semantic_init();

/**
 * @brief Destructor for parser semantic
 *
 * @param ParserSemantic** parser
 */
void parser_semantic_free(ParserSemantic** parser);

/**
 * @brief Find variable in symbol variable
 *
 * @param parser_semantic
 * @param token
 * @return SymbolVariable*
 */
SymbolVariable* parser_semantic_expect_symbol_variable(ParserSemantic* parser_semantic, Token token);

/**
 * @brief Add Variable to the actual symbol table
 *
 * @param parser_semantic
 * @param name
 * @param data_type
 * @return bool
 */
bool parser_semantic_add_symbol_variable(ParserSemantic* parser_semantic, char* name, short data_type);

/**
 * @brief If the actual action is ACTUAL_ACTION__FUNCTION_DECLARATION, then set return data type for actual function,
 *        If the actual action is ACTUAL_ACTION__FUNCTION_DEFINITION, then check return data type in symbol table
 *
 * @param parser_semantic
 * @param token_type
 * @return bool
 */
bool parser_semantic_function_return_data_type(ParserSemantic* parser_semantic, TokenType token_type);

#endif //_PARSER_SEMANTIC_H