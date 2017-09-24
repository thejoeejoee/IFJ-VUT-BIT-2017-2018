#ifndef _LEXER_FSM_H
#define _LEXER_FSM_H

#include <stdbool.h>
#include "char_stack.h"
#include "dynamic_string.h"

#define LEXER_FSM_STREAM_BUFFER_DEFAULT_LENGHT 16

typedef int (* lexer_input_stream_f)();

typedef struct lexer_fsm_t {
    CharStack* stack;
    String stream_buffer;
    lexer_input_stream_f input_stream;
} LexerFSM;

typedef enum {

    // Start state
    LEX_FSM__INIT,

    // Comments
    LEX_FSM__COMMENT_LINE,
    LEX_FSM__SLASH,
    LEX_FSM__COMMENT_BLOCK,
    LEX_FSM__COMMENT_BLOCK_END,


    // Sharp brackets
    LEX_FSM__LEFT_SHARP_BRACKET,
    LEX_FSM__RIGHT_SHARP_BRACKET,

    // Other unfinished states
    LEX_FSM__IDENTIFIER_UNFINISHED,
    LEX_FSM__INTEGER_LITERAL_UNFINISHED,
    LEX_FSM__DOUBLE_DOT,
    LEX_FSM__DOUBLE_UNFINISHED,
    LEX_FSM__DOUBLE_E,
    LEX_FSM__DOUBLE_E_UNFINISHED,

    // String unfinished states
    LEX_FSM__STRING_EXC,
    LEX_FSM__STRING_LOAD,
    LEX_FSM__STRING_SLASH,

    // FINAL STATES
    LEX_FSM__ADD,
    LEX_FSM__SUBTRACT,
    LEX_FSM__DIVIDE,
    LEX_FSM__MULTIPLY,
    LEX_FSM__LEFT_BRACKET,
    LEX_FSM__RIGHT_BRACKET,
    LEX_FSM__INTEGER_LITERAL_FINISHED,
    LEX_FSM__DOUBLE_FINISHED,
    LEX_FSM__STRING_VALUE,

    // Relation operators
    LEX_FSM__SMALLER,
    LEX_FSM__BIGGER,
    LEX_FSM__SMALLER_EQUAL,
    LEX_FSM__BIGGER_EQUAL,
    LEX_FSM__IDENTIFIER_FINISHED,

    // Reserve words
    LEX_FSM__AS,
    LEX_FSM__ASC,
    LEX_FSM__DECLARE,
    LEX_FSM__DIM,
    LEX_FSM__DO,
    LEX_FSM__DOUBLE,
    LEX_FSM__ELSE,
    LEX_FSM__END,
    LEX_FSM__CHR,
    LEX_FSM__FUNCTION,
    LEX_FSM__IF,
    LEX_FSM__INPUT,
    LEX_FSM__INTEGER,
    LEX_FSM__LENGTH,
    LEX_FSM__LOOP,
    LEX_FSM__PRINT,
    LEX_FSM__RETURN,
    LEX_FSM__SCOPE,
    LEX_FSM__STRING,
    LEX_FSM__SUBSTR,
    LEX_FSM__THEN,
    LEX_FSM__WHILE,
    LEX_FSM__AND,
    LEX_FSM__BOOLEAN,
    LEX_FSM__CONTINUE,
    LEX_FSM__ELSEIF,
    LEX_FSM__EXIT,
    LEX_FSM__FALSE,
    LEX_FSM__FOR,
    LEX_FSM__NEXT,
    LEX_FSM__NOT,
    LEX_FSM__OR,
    LEX_FSM__SHARED,
    LEX_FSM__STATIC,
    LEX_FSM__TRUE,

    // Error state
    LEX_FSM__LEG_SHOT

} LexerFSMState;

/**
 * @brief Constructor for LexerFSM
 *
 * @param input_stream Callable to gen chars from stream.
 * @return LexerFSM*
 */
LexerFSM *lexer_fsm_init(lexer_input_stream_f input_stream);

/**
 * @brief Destructor for LexerFSM
 *
 * @param lexer_fsm
 */
void lexer_fsm_free(LexerFSM** lexer_fsm);

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
LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, LexerFSM *lexer_fsm);

/**
 * @brief Find out if the state is final
 *
 * @param LexerFSMState state actual state
 * @return bool
 */
bool lexer_fsm_is_final_state(LexerFSMState state);

#endif // _LEXER_FSM_H