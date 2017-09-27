#ifndef _LEXER_H
#define _LEXER_H

#include "token.h"
#include "lexer_fsm.h"

/**
 * @brief Representation of lexical analyzer
 */
typedef struct lexer_t {
    lexer_input_stream_f input_stream; // Pointer to function which stream chars
    LexerFSM *lexer_fsm; // Instance of final state machine

    Token* stack_token; // Stack for token
} Lexer;


/**
 * @brief Constructor for lexer
 *
 * @return *lexer_fsm Pointer to lexer
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

/**
 * @brief Push token to the stack
 *
 * @param Lexer* lexer Pointer to instance of lexer
 * @param Token* token Pointer to token
 */
void lexer_return_token(Lexer* lexer, Token* token);

#endif //_LEXER_H
