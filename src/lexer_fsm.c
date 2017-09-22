#include <stdbool.h>
#include <ctype.h>
#include "lexer_fsm.h"
#include "debug.h"
#include "char_stack.h"

LexerFSM *lexer_fsm_init() {
    LexerFSM* lexer = (LexerFSM*) memory_alloc(sizeof(LexerFSM));
    NULL_POINTER_CHECK(lexer, NULL);
    CharStack* stack = char_stack_init();
    lexer->stack = stack;
    lexer->char_position = -1;

    return lexer;
}

LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, lexer_input_stream_f input_stream, LexerFSM *lexer_fsm) {
    NULL_POINTER_CHECK(input_stream, LEX_FSM__LEG_SHOT);
    NULL_POINTER_CHECK(lexer_fsm, LEX_FSM__LEG_SHOT);

    int c = char_stack_pop(lexer_fsm->stack);

    if(c == EOF)
        c = input_stream();


    switch(prev_state) {
        case LEX_FSM__INIT:

            // If it is a white space, we ignore it
            if(lexer_is_white_space(c))
                return LEX_FSM__INIT;

            if(c == '_' || isalpha(c)) {
                lexer_fsm_add_identifier_symbol(lexer_fsm, tolower(c));
                return LEX_FSM__IDENTIFIER_UNFINISHED;
            }

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
                case '<':
                    return LEX_FSM__LEFT_SHARP_BRACKET;
                case '>':
                    return LEX_FSM__RIGHT_SHARP_BRACKET;

                default:
                    break;
            }
            break;

        case LEX_FSM__LEFT_SHARP_BRACKET:
            if(c == '=')
                return LEX_FSM__SMALLER_EQUAL;
            else{
                char_stack_push(lexer_fsm->stack, c);
                return LEX_FSM__SMALLER;
            }
        case LEX_FSM__RIGHT_SHARP_BRACKET:
            if(c == '=')
                return LEX_FSM__BIGGER_EQUAL;
            else{
                char_stack_push(lexer_fsm->stack, c);
                return LEX_FSM__BIGGER;
            }

        case LEX_FSM__IDENTIFIER_UNFINISHED:
            if(c == '_' || isdigit(c) || isalpha(c)) {
                lexer_fsm_add_identifier_symbol(lexer_fsm, tolower(c));
                return LEX_FSM__IDENTIFIER_UNFINISHED;
            }
            else {
                char_stack_push(lexer_fsm->stack, tolower(c));
                lexer_fsm_end_identifier_name(lexer_fsm);

                return lexer_fsm_get_identifier_type(lexer_fsm->identifier_name);
            }

        case LEX_FSM__SLASH:
            if(c == '\'')
                return LEX_FSM__COMMENT_BLOCK;
            else {
                char_stack_push(lexer_fsm->stack, c);
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

LexerFSMState lexer_fsm_get_identifier_type(char *name) {
    // TODO: Macro is faster....

    if(strcmp(name, "if") == 0) {
        return LEX_FSM__IF;
    }

    if(strcmp(name, "else") == 0) {
        return LEX_FSM__ELSE;
    }

    if(strcmp(name, "begin") == 0) {
        return LEX_FSM__BEGIN;
    }

    if(strcmp(name, "end") == 0) {
        return LEX_FSM__END;
    }

    return LEX_FSM__IDENTIFIER_FINISHED;
}

bool lexer_fsm_add_identifier_symbol(LexerFSM *lexer_fsm, char c) {
    lexer_fsm->identifier_name[++(lexer_fsm->char_position)] = c;
    return true;
}

void lexer_fsm_end_identifier_name(LexerFSM *lexer_fsm) {
    lexer_fsm->identifier_name[++(lexer_fsm->char_position)] = '\0';
    lexer_fsm->char_position = -1;
}



bool lexer_fsm_is_final_state(LexerFSMState state) {
    // TODO: inline of macro to better performance
    switch(state) {
        case LEX_FSM__IF:
        case LEX_FSM__ELSE:
        case LEX_FSM__BEGIN:
        case LEX_FSM__END:
        case LEX_FSM__IDENTIFIER_FINISHED:
        case LEX_FSM__COMMENT_LINE:
        case LEX_FSM__ADD:
        case LEX_FSM__SUBTRACT:
        case LEX_FSM__MULTIPLY:
        case LEX_FSM__DIVIDE:
        case LEX_FSM__LEFT_BRACKET:
        case LEX_FSM__RIGHT_BRACKET:
        case LEX_FSM__SMALLER:
        case LEX_FSM__BIGGER:
        case LEX_FSM__SMALLER_EQUAL:
        case LEX_FSM__BIGGER_EQUAL:
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