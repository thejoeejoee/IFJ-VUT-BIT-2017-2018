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

#endif //_LEXER_H
