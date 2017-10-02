#ifndef _PARSER_SEMANTIC_H
#define _PARSER_SEMANTIC_H

#include "symbol_register.h"

typedef struct parser_semantic_t {
    SymbolRegister* register_;

    // TODO: actual processed function identifier
} ParserSemantic;

ParserSemantic* parser_semantic_init();

void parser_semantic_free(ParserSemantic** parser);

#endif //_PARSER_SEMANTIC_H
