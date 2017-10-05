#ifndef _LEXER_H
#define _LEXER_H

#include "token.h"
#include "lexer_fsm.h"
#include "error.h"

/**
 * @brief Representation of lexical analyzer
 */
typedef struct lexer_t {
    lexer_input_stream_f input_stream; // Pointer to function which stream chars
    LexerFSM* lexer_fsm; // Instance of final state machine

    bool is_token_rewind;
    Token rewind_token; // buffered token
    ErrorReport error_report; // Error report
} Lexer;


/**
 * @brief Constructor for lexer
 *
 * @return *lexer_fsm Pointer to lexer
 */
Lexer* lexer_init(lexer_input_stream_f input_stream);

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
Token lexer_next_token(Lexer* lexer);

/**
 * @brief Store token additional data from stream_buffer to .data ptr - only for identifiers and number, string and double literals.
 * @param lexer lexer pointer
 * @param token token to process
 * @return pointer to data block with additional data
 */
char* lexer_store_token_data(const Lexer* lexer, Token token);

/**
 * @brief Push token to the stack (only one token can be rewinded at time)
 *
 * @param Lexer* lexer Pointer to instance of lexer
 * @param Token* token Pointer to token
 */
void lexer_rewind_token(Lexer* lexer, Token token);

#endif //_LEXER_H
