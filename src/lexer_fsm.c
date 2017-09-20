#include <stdbool.h>
#include "lexer_fsm.h"
#include "debug.h"

LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, lexer_input_stream_f input_stream) {
    NULL_POINTER_CHECK(input_stream, LEX_FSM__LEG_SHOT);

    int c;
    static int stack[LEXER_FSM_STACK_SIZE];
    static int stack_head = -1;
    if (stack_head != -1)
        c = stack[stack_head--];
    else
        c = input_stream();


    switch (prev_state) {
        case LEX_FSM__INIT:
            switch (c) {
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

        case LEX_FSM__SLASH:
            if (c == '\'')
                return LEX_FSM__COMMENT_BLOCK;
            else {
                stack[++stack_head] = c;
                return LEX_FSM__DIVIDE;
            }

        case LEX_FSM__COMMENT_LINE:
            if (c != '\n')
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

bool is_final_state(LexerFSMState state) {
    switch (state) {
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