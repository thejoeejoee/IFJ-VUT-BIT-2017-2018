#ifndef _LEXER_FSM_H
#define _LEXER_FSM_H

#include <stdbool.h>
#include "char_stack.h"
#include "dynamic_string.h"

#define LEXER_FSM_STREAM_BUFFER_DEFAULT_LENGTH 2

/**
 * @brief Pointer to function, which streams the characters
 */
typedef int (* lexer_input_stream_f)();

/**
 * @brief List of lexer errors
 */
typedef enum {
    LEXER_ERROR__NO_ERROR,
    LEXER_ERROR__STRING_FORMAT,
    LEXER_ERROR__DOUBLE_FORMAT,
    LEXER_ERROR__ERROR_LEXEM,
} LexerError;

/**
 * @brief Representation of the FSM that is part of the lexical analyzer
 */
typedef struct lexer_fsm_t {

    CharStack* stack; // Instance of stack for returning symbols back
    String* stream_buffer; // Dynamic string for progressive compilation value
    lexer_input_stream_f input_stream; // Pointer to function which stream chars

    char numeric_char_value[4]; // Stack for numeric value of char
    short numeric_char_position; // Head of stack for numeric value of char

    size_t line; // Actual line

    LexerError lexer_error; // Error code
} LexerFSM;

/**
 * @brief List of FSM states
 */
typedef enum {

    // Start state

    LEX_FSM__INIT = 0,

    LEX_FSM__AMP,
    LEX_FSM__BINARY_START,
    LEX_FSM__BINARY_UNFINISHED,
    LEX_FSM__OCTA_START,
    LEX_FSM__OCTA_UNFINISHED,
    LEX_FSM__HEXA_START,
    LEX_FSM__HEXA_UNFINISHED,

    // Unfinished states for operators
            LEX_FSM__ADD_UNFINISHED,
    LEX_FSM__SUBTRACT_UNFINISHED,
    LEX_FSM__MULTIPLY_UNFINISHED,
    LEX_FSM__INTEGER_DIVIDE_UNFINISHED,


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
    LEX_FSM__STRING_NUMERIC_CHAR,

    // FINAL STATES
    // 128 is operator class
    LEX_FSM__ADD,
    LEX_FSM__SUBTRACT,
    LEX_FSM__DIVIDE,
    LEX_FSM__INTEGER_DIVIDE,
    LEX_FSM__MULTIPLY,
    LEX_FSM__INTEGER_LITERAL_FINISHED,
    LEX_FSM__DOUBLE_FINISHED,
    LEX_FSM__STRING_VALUE,
    LEX_FSM__SMALLER_BIGGER,
    LEX_FSM__EOL,
    LEX_FSM__SMALLER,
    LEX_FSM__BIGGER,
    LEX_FSM__SMALLER_EQUAL,
    LEX_FSM__BIGGER_EQUAL,
    LEX_FSM__IDENTIFIER_FINISHED,
    LEX_FSM__EQUAL,
    LEX_FSM__SEMICOLON,
    LEX_FSM__COMMA,

    // Brackets
    // 256 is brackets class
    LEX_FSM__LEFT_BRACKET,
    LEX_FSM__RIGHT_BRACKET,

    // Reserve words

    LEX_FSM__AS,
    LEX_FSM__ASC,
    LEX_FSM__DECLARE,
    LEX_FSM__DIM,
    LEX_FSM__DO,
    LEX_FSM__ELSE,
    LEX_FSM__END,
    LEX_FSM__CHR,
    LEX_FSM__FUNCTION,
    LEX_FSM__IF,
    LEX_FSM__INPUT,
    LEX_FSM__LENGTH,
    LEX_FSM__LOOP,
    LEX_FSM__PRINT,
    LEX_FSM__RETURN,
    LEX_FSM__SCOPE,
    LEX_FSM__SUBSTR,
    LEX_FSM__THEN,
    LEX_FSM__WHILE,
    LEX_FSM__AND,
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
    LEX_FSM__EOF,
    LEX_FSM__ERROR,

    LEX_FSM__INTEGER = 256 + 1,
    LEX_FSM__DOUBLE = 256 + 2,
    LEX_FSM__BOOLEAN = 256 + 3,
    LEX_FSM__STRING = 256 + 4,

    LEX_FSM__ASSIGN_ADD = 512 + 1,
    LEX_FSM__ASSIGN_SUB = 512 + 2,
    LEX_FSM__ASSIGN_MULTIPLY = 512 + 3,
    LEX_FSM__ASSIGN_INT_DIVIDE = 512 + 4,
    LEX_FSM__ASSIGN_DIVIDE = 512 + 5,

    // Error state


} LexerFSMState;

/**
 * @brief Constructor for LexerFSM
 *
 * @param input_stream Callable to gen chars from stream.
 * @return LexerFSM*
 */
LexerFSM* lexer_fsm_init(lexer_input_stream_f input_stream);

/**
 * @brief Destructor for LexerFSM
 *
 * @param lexer_fsm
 */
void lexer_fsm_free(LexerFSM** lexer_fsm);

/**
 * @brief Get identifier type from name
 *
 * @param char* name
 * @return LexerFSMState
 */
LexerFSMState lexer_fsm_get_identifier_state(const char* name);

/**
 * @brief Get next state from prev state and next symbol
 *
 * @param LexerFSMState prev_state
 * @param lexer_input_stream_f input_stream
 * @return LexerFSMState Next state
 */
LexerFSMState lexer_fsm_next_state(LexerFSM* lexer_fsm, LexerFSMState prev_state);

/**
 * @brief Find out if the state is final
 *
 * @param LexerFSMState state actual state
 * @return bool
 */
bool lexer_fsm_is_final_state(LexerFSMState state);

#endif // _LEXER_FSM_H
