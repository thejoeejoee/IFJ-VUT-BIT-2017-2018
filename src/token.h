#ifndef _TOKEN_H
#define _TOKEN_H

typedef enum {
    TOKEN_UNKNOWN = 0,

    TOKEN_MULTIPLY,
    TOKEN_ADD,
    TOKEN_DIVIDE,
    TOKEN_BEGIN,
} TokenType;

/**
 * @brief Represents a token
 */
typedef struct token_t {
    TokenType type;
} Token;


#endif //_TOKEN_H
