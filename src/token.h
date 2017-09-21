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
    TOKEN_BEGIN,
} TokenType;

/**
 * @brief Represents a token
 */
typedef struct token_t {
    TokenType type;
} Token;


#endif //_TOKEN_H
