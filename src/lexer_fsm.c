#include "lexer_fsm.h"
#include "debug.h"


LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, lexer_input_stream_f input_stream) {
    NULL_POINTER_CHECK(input_stream, LEX_FSM__LEG_SHOT);

    int c;
    static int stack[64];
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
                    stack[++stack_head] = c;
                    c = input_stream();
                    switch (c) {
                        case '\'':
                            return LEX_FSM__COMMENT_BLOCK;
                        default:
                            return LEX_FSM__DIVIDE;
                    }
                default:
                    break;
            }
            break;
        case LEX_FSM__COMMENT_LINE:
            if (c != '\n')
                return LEX_FSM__COMMENT_LINE;
            return LEX_FSM__INIT;

        default:
            break;
    }

    // TODO never happened, Chuck Norris state
    return LEX_FSM__LEG_SHOT;
}