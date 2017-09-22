#ifndef _LEXER_H
#define _LEXER_H

#include "token.h"
#include "lexer_fsm.h"

typedef struct lexer_t {
    lexer_input_stream_f input_stream;
    LexerFSM *lexer_fsm;
} Lexer;


/**
 * @brief Constructor for lexer
 *
 * @return Pointer to lexer
 */
Lexer* lexer_init(lexer_input_stream_f);

/**
 * @brief Free lexer also with stack from memory.
 */
void lexer_free(Lexer** lexer);

/**
 * @brief Get next token from lexer
 *
 * @param Lexer* lexer Pointer to lexer
 * @return Token* Pointer to next token
 */
Token* lexer_next_token(Lexer* lexer);

#endif //_LEXER_H
