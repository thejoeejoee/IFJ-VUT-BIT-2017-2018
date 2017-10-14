#ifndef _PARSER_EXPR_INTERNAL_H
#define _PARSER_EXPR_INTERNAL_H

#include "lexer.h"
#include "llist.h"

extern const int prec_table_size;
extern const char* prec_table;

typedef enum {
    // Terminals
    EXPR_TOKEN_UNARY_MINUS = 1,
    EXPR_TOKEN_MULTIPLY = 2,
    EXPR_TOKEN_DIVIDE = 3,
    EXPR_TOKEN_INTEGER_DIVIDE = 4,
    EXPR_TOKEN_PLUS = 5,
    EXPR_TOKEN_MINUS = 6,
    EXPR_TOKEN_EQUAL = 7,
    EXPR_TOKEN_NOT_EQUAL = 8,
    EXPR_TOKEN_LESSER = 9,
    EXPR_TOKEN_LESSER_OR_EQUAL = 10,
    EXPR_TOKEN_GREATHER_OR_EQUAL = 11,
    EXPR_TOKEN_GREATHER = 12,
    EXPR_TOKEN_NOT = 13,
    EXPR_TOKEN_AND = 14,
    EXPR_TOKEN_OR = 15,

    EXPR_TOKEN_LEFT_BRACKET = 16,
    EXPR_TOKEN_RIGHT_BRACKET = 17,

    EXPR_TOKEN_FUNCTION_CALL = 18,
    EXPR_TOKEN_COMMA = 19,

    EXPR_TOKEN_IDENTIFIER = 20,
    EXPR_TOKEN_$ = 21,

    EXPR_TOKEN_BOOLEAN_LITERAL = 22,
    EXPR_TOKEN_INTEGER_LITERAL = 23,
    EXPR_TOKEN_DOUBLE_LITERAL = 24,
    EXPR_TOKEN_STRING_LITERAL = 25,

    // Mask
    EXPR_TERMINALS_MASK = 127, //for internal usage

    // Others
    EXPR_EXPRESSION = 128,    // E
    EXPR_LEFT_SHARP = 129, EXPR_SHIFT = EXPR_LEFT_SHARP,      // <
    EXPR_RIGHT_SHARP = 130, EXPR_REDUCE = EXPR_RIGHT_SHARP,   // >
    EXPR_SAME = 131,    // =
    EXPR_TOKENCHANGE = 132, // * so called 'token change', when changing from 'identifier token' and 'left bracket token' to 'function call token'

    EXPR_UNKNOWN = 133  // unknown token or undefined precedence
} ExprTokenType;

typedef unsigned ExprIdx;

typedef union expr_data_u {
    ExprIdx idx;
    char* s;
} ExprData;

typedef struct expr_token_t {
    ExprTokenType type;
    ExprData data;
} ExprToken;


ExprToken* expr_get_precedence(ExprToken *a, ExprToken *b);
ExprToken* load_expr_token(Lexer* lexer, Token* last_token);
void expr_token_free(ExprToken *t);
int expr_llist_type_cmp(void* a, void* b);
void expr_llist_free(void* data);
ExprToken* create_expr_token(ExprTokenType type);
ExprToken* create_expression(ExprIdx index);
ExprToken* expr_last_terminal(LList *expr_token_buffer);
void expr_llist_append_after_last_terminal(LList *expr_token_buffer, ExprToken* token);
void expr_token_update_unary(ExprToken* minus, const ExprToken* previous);


#endif //_PARSER_EXPR_INTERNAL_H
