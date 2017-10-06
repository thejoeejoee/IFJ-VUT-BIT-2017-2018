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

void parser_semantic_set_action(ParserSemantic* parser_semantic, ActualAction actual_action);

bool parser_semantic_add_function(ParserSemantic* parserSemantic, char* name);

ParserSemantic* parser_semantic_init();

void parser_semantic_free(ParserSemantic** parser);

SymbolVariable* parser_semantic_expect_symbol_variable(ParserSemantic* parser_semantic, Token token);

bool parser_semantic_add_symbol_variable(ParserSemantic* parser_semantic, char* name, short data_type);

void parser_semantic_add_function_return_data_type(ParserSemantic* parser_semantic, TokenType token_type);

#endif //_PARSER_SEMANTIC_H