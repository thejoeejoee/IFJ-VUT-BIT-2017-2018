#ifndef _PARSER_SEMANTIC_H
#define _PARSER_SEMANTIC_H

#include "symbol_register.h"
#include "token.h"
#include "error.h"

typedef struct parser_semantic_t {
    SymbolRegister* register_;

    ErrorReport error_report;
    // TODO: actual processed function identifier
} ParserSemantic;

ParserSemantic* parser_semantic_init();

void parser_semantic_free(ParserSemantic** parser);

SymbolVariable* parser_semantic_expect_symbol_variable(ParserSemantic* parser_semantic, Token token);

bool parser_semantic_add_symbol_variable(ParserSemantic* parser_semantic, char* name, short data_type);

#endif //_PARSER_SEMANTIC_H