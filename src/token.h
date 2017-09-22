#ifndef _TOKEN_H
#define _TOKEN_H

#include "lexer_fsm.h"

typedef enum {
    TOKEN_UNKNOWN = 0,

    // Token types must have the same value as final state
    TOKEN_MULTIPLY = LEX_FSM__MULTIPLY,
    TOKEN_ADD = LEX_FSM__ADD,
    TOKEN_DIVIDE = LEX_FSM__DIVIDE,
    TOKEN_SUBTRACT = LEX_FSM__SUBTRACT,
    TOKEN_LEFT_BRACKET = LEX_FSM__LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET = LEX_FSM__RIGHT_BRACKET,
    TOKEN_IDENTIFIER = LEX_FSM__IDENTIFIER_FINISHED,
    TOKEN_BIGGER = LEX_FSM__BIGGER,
    TOKEN_SMALLER = LEX_FSM__SMALLER,
    TOKEN_BIGGER_EQUAL = LEX_FSM__BIGGER_EQUAL,
    TOKEN_SMALLER_EQUAL = LEX_FSM__SMALLER_EQUAL,
    TOKEN_IF = LEX_FSM__IF,
    TOKEN_ELSE = LEX_FSM__ELSE,
    TOKEN_BEGIN = LEX_FSM__BEGIN,
    TOKEN_END = LEX_FSM__END,
} TokenType;

/**
 * @brief Represents a token
 */
typedef struct token_t {
    TokenType type;
} Token;


#endif //_TOKEN_H
