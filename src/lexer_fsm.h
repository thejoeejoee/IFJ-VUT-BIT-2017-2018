#ifndef _LEXER_FSM_H
#define _LEXER_FSM_H

#include <stdbool.h>
#include "char_stack.h"

#define LEXER_FSM_STACK_SIZE 128

typedef int (* lexer_input_stream_f)();

typedef struct lexer_fsm_t {
    CharStack* stack;
    char identifier_name[256];
    int char_position;
} LexerFSM;

typedef enum {

    // Start state
    LEX_FSM__INIT,

    // Comments
    LEX_FSM__COMMENT_LINE,
    LEX_FSM__SLASH,
    LEX_FSM__COMMENT_BLOCK,
    LEX_FSM__COMMENT_BLOCK_END,

    // Operations in mathematical expressions
    LEX_FSM__ADD,
    LEX_FSM__SUBTRACT,
    LEX_FSM__DIVIDE,
    LEX_FSM__MULTIPLY,

    // Brackets
    LEX_FSM__LEFT_BRACKET,
    LEX_FSM__RIGHT_BRACKET,

    // Sharp brackets
    LEX_FSM__LEFT_SHARP_BRACKET,
    LEX_FSM__RIGHT_SHARP_BRACKET,

    // Relation operators
    LEX_FSM__SMALLER,
    LEX_FSM__BIGGER,
    LEX_FSM__SMALLER_EQUAL,
    LEX_FSM__BIGGER_EQUAL,

    // Identifier states
    LEX_FSM__IDENTIFIER_UNFINISHED,
    LEX_FSM__IDENTIFIER_FINISHED,

    // Reserve words
    LEX_FSM__IF,
    LEX_FSM__ELSE,
    LEX_FSM__BEGIN,
    LEX_FSM__END,

    // Error state
    LEX_FSM__LEG_SHOT

} LexerFSMState;

/**
 * @brief Constructor for LexerFSM
 *
 * @return LexerFSM*
 */
LexerFSM *lexer_fsm_init();

/**
 * @brief Add identifier symbol to the symbol stack
 * @param lexer_fsm
 * @param char c
 * @return bool
 */
bool lexer_fsm_add_identifier_symbol(LexerFSM *lexer_fsm, char c);

/**
 * @bried add \0 to the end of identifier name
 * @param lexer_fsm
 */
void lexer_fsm_end_identifier_name(LexerFSM *lexer_fsm);

/**
 * @brief Get identifier type from name
 *
 * @param char* name
 * @return LexerFSMState
 */
LexerFSMState lexer_fsm_get_identifier_type(char *name);

/**
 * @brief Get next state from prev state and next symbol
 *
 * @param LexerFSMState prev_state
 * @param lexer_input_stream_f input_stream
 * @return LexerFSMState
 */
LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, lexer_input_stream_f input_stream, LexerFSM *lexer_fsm);

/**
 * @brief Find out if the state is final
 *
 * @param LexerFSMState state actual state
 * @return bool
 */
bool lexer_fsm_is_final_state(LexerFSMState state);

/**
 * @brief Find out if the character is white space
 *
 * @param char c
 * @return bool
 */
bool lexer_is_white_space(char c);

#endif // _LEXER_FSM_H