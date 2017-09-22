#include <stdbool.h>
#include <ctype.h>
#include "lexer_fsm.h"
#include "debug.h"
#include "char_stack.h"

LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, lexer_input_stream_f input_stream, CharStack* stack) {
    NULL_POINTER_CHECK(input_stream, LEX_FSM__LEG_SHOT);
    NULL_POINTER_CHECK(stack, LEX_FSM__LEG_SHOT);

    int c = char_stack_pop(stack);

    if(c == EOF)
        c = input_stream();


    switch(prev_state) {
        case LEX_FSM__INIT:

            // If it is a white space, we ignore it
            if(lexer_is_white_space(c))
                return LEX_FSM__INIT;

            if(c == '_' || isalpha(c))
                return LEX_FSM__IDENTIFIER_UNFINISHED;

            if(c == 'a')
                return LEX_FSM__IDENTIFIER_UNFINISHED;

            switch(c) {
                case '\'':
                    return LEX_FSM__COMMENT_LINE;
                case '/':
                    // It can be comment or divide symbol
                    return LEX_FSM__SLASH;

                case '+':
                    return LEX_FSM__ADD;
                case '-':
                    return LEX_FSM__SUBTRACT;
                case '*':
                    return LEX_FSM__MULTIPLY;

                case '(':
                    return LEX_FSM__LEFT_BRACKET;
                case ')':
                    return LEX_FSM__RIGHT_BRACKET;

                default:
                    break;
            }
            break;

        case LEX_FSM__IDENTIFIER_UNFINISHED:
            if(c == '_' || isdigit(c) || isalpha(c))
                return LEX_FSM__IDENTIFIER_UNFINISHED;
            else
                return LEX_FSM__IDENTIFIER_FINISHED;

        case LEX_FSM__SLASH:
            if(c == '\'')
                return LEX_FSM__COMMENT_BLOCK;
            else {
                char_stack_push(stack, c);
                return LEX_FSM__DIVIDE;
            }

        case LEX_FSM__COMMENT_LINE:
            if(c != '\n')
                return LEX_FSM__COMMENT_LINE;
            return LEX_FSM__INIT;

        case LEX_FSM__COMMENT_BLOCK:
            if(c == '\'')
                return LEX_FSM__COMMENT_BLOCK_END;
            else
                return LEX_FSM__COMMENT_BLOCK;

        case LEX_FSM__COMMENT_BLOCK_END:
            if(c == '/')
                return LEX_FSM__INIT;
            else
                return LEX_FSM__COMMENT_BLOCK;

        default:
            break;
    }

    // TODO never happened, Chuck Norris state
    return LEX_FSM__LEG_SHOT;
}

bool lexer_fsm_is_final_state(LexerFSMState state) {
    // TODO: inline of macro to better performance
    switch(state) {
        case LEX_FSM__IDENTIFIER_FINISHED:
        case LEX_FSM__COMMENT_LINE:
        case LEX_FSM__ADD:
        case LEX_FSM__SUBTRACT:
        case LEX_FSM__MULTIPLY:
        case LEX_FSM__DIVIDE:
        case LEX_FSM__LEFT_BRACKET:
        case LEX_FSM__RIGHT_BRACKET:
            return true;
        default:
            return false;
    }
}

bool lexer_is_white_space(char c) {
    // TODO: inline or macro version has better performance
    switch(c) {
        case ' ':
        case '\n':
        case '\t':
            return true;
        default:
            return false;
    }
}