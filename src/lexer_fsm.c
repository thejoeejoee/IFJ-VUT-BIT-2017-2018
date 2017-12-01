#include <stdbool.h>
#include <ctype.h>
#include "lexer_fsm.h"
#include "debug.h"
#include "char_stack.h"
#include "dynamic_string.h"

#define REWIND_CHAR(c) char_stack_push(lexer_fsm->stack, (char) (c))
#define STORE_CHAR(c) string_append_c(lexer_fsm->stream_buffer, (char) (c));


LexerFSM* lexer_fsm_init(lexer_input_stream_f input_stream) {
    NULL_POINTER_CHECK(input_stream, NULL);
    LexerFSM* lexer_fsm = (LexerFSM*) memory_alloc(sizeof(LexerFSM));
    NULL_POINTER_CHECK(lexer_fsm, NULL);
    CharStack* stack = char_stack_init();
    lexer_fsm->stream_buffer = string_init_with_capacity(LEXER_FSM_STREAM_BUFFER_DEFAULT_LENGTH);
    lexer_fsm->stack = stack;
    lexer_fsm->input_stream = input_stream;
    lexer_fsm->numeric_char_position = -1;
    lexer_fsm->lexer_error = LEXER_ERROR__NO_ERROR;

    lexer_fsm->line = 1;

    return lexer_fsm;
}

void lexer_fsm_free(LexerFSM** lexer_fsm) {
    NULL_POINTER_CHECK(lexer_fsm,);
    NULL_POINTER_CHECK(*lexer_fsm,);
;
    char_stack_free(&(*lexer_fsm)->stack);
    string_free(&(*lexer_fsm)->stream_buffer);
    memory_free(*lexer_fsm);
    *lexer_fsm = NULL;
}

LexerFSMState lexer_fsm_next_state(LexerFSM* lexer_fsm, LexerFSMState prev_state) {
    NULL_POINTER_CHECK(lexer_fsm, LEX_FSM__ERROR);

    // stored chars in stack from before loops have priority
    int c = char_stack_pop(lexer_fsm->stack);

    if(c == EOF)
        c = lexer_fsm->input_stream();

    switch(prev_state) {
        // Starting state
        case LEX_FSM__INIT:
            // If it is a white space, we ignore it
            if(c == '\n') {
                lexer_fsm->line++;
                return LEX_FSM__EOL;
            }

            if(isspace(c))
                return LEX_FSM__INIT;

            if(c == '_' || isalpha(c)) {
                STORE_CHAR(tolower(c));
                return LEX_FSM__IDENTIFIER_UNFINISHED;
            }
            if(isdigit(c)) {
                STORE_CHAR(c);
                return LEX_FSM__INTEGER_LITERAL_UNFINISHED;
            }

            switch(c) {
                case '!':
                    return LEX_FSM__STRING_EXC;
                case '\'':
                    return LEX_FSM__COMMENT_LINE;
                case '/':
                    return LEX_FSM__SLASH;
                case '\\':
                    return LEX_FSM__INTEGER_DIVIDE_UNFINISHED;
                case '+':
                    return LEX_FSM__ADD_UNFINISHED;
                case '-':
                    return LEX_FSM__SUBTRACT_UNFINISHED;
                case '*':
                    return LEX_FSM__MULTIPLY_UNFINISHED;
                case '(':
                    return LEX_FSM__LEFT_BRACKET;
                case ')':
                    return LEX_FSM__RIGHT_BRACKET;
                case '<':
                    return LEX_FSM__LEFT_SHARP_BRACKET;
                case '>':
                    return LEX_FSM__RIGHT_SHARP_BRACKET;
                case '=':
                    return LEX_FSM__EQUAL;
                case ';':
                    return LEX_FSM__SEMICOLON;
                case ',':
                    return LEX_FSM__COMMA;
                case '&':
                    return LEX_FSM__AMP;
                case EOF:
                    return LEX_FSM__EOF;

                default:
                    break;
            }
            break;

            // Binary integers
        case LEX_FSM__AMP:

            if(tolower(c) == 'b') {
                STORE_CHAR(tolower(c));
                return LEX_FSM__BINARY_START;
            }

            if(tolower(c) == 'o') {
                STORE_CHAR(tolower(c));
                return LEX_FSM__OCTA_START;
            }

            if(tolower(c) == 'h') {
                STORE_CHAR(tolower(c));
                return LEX_FSM__HEXA_START;
            }

            return LEX_FSM__ERROR;

        case LEX_FSM__BINARY_START:

            if(c == '0' || c == '1') {
                STORE_CHAR(c);
                return LEX_FSM__BINARY_UNFINISHED;
            }
            REWIND_CHAR(c);
            return LEX_FSM__INTEGER_LITERAL_FINISHED;

        case LEX_FSM__BINARY_UNFINISHED:

            if(c == '0' || c == '1') {
                STORE_CHAR(c);
                return LEX_FSM__BINARY_UNFINISHED;
            }

            REWIND_CHAR(c);
            return LEX_FSM__INTEGER_LITERAL_FINISHED;

        case LEX_FSM__OCTA_START:

            if(c >= (int) '0' && c <= (int) '7') {
                STORE_CHAR(c);
                return LEX_FSM__OCTA_UNFINISHED;
            }
                REWIND_CHAR(c);
                return  LEX_FSM__INTEGER_LITERAL_FINISHED;

        case LEX_FSM__OCTA_UNFINISHED:

            if(c >= (int) '0' && c <= (int) '7') {
                STORE_CHAR(c);
                return LEX_FSM__OCTA_UNFINISHED;
            }

            REWIND_CHAR(c);
            return LEX_FSM__INTEGER_LITERAL_FINISHED;

        case LEX_FSM__HEXA_START:

            if((c >= (int) '0' && c <= (int) '9') || (tolower(c) >= (int) 'a' && tolower(c) <= (int) 'f')) {
                STORE_CHAR(tolower(c));
                return LEX_FSM__HEXA_UNFINISHED;
            }
            REWIND_CHAR(c);
            return LEX_FSM__INTEGER_LITERAL_FINISHED;

        case LEX_FSM__HEXA_UNFINISHED:

            if((c >= (int) '0' && c <= (int) '9') || (tolower(c) >= (int) 'a' && tolower(c) <= (int) 'f')) {
                STORE_CHAR(tolower(c));
                return LEX_FSM__HEXA_UNFINISHED;
            }

            REWIND_CHAR(c);
            return LEX_FSM__INTEGER_LITERAL_FINISHED;

            // Operator unfinished states
        case LEX_FSM__ADD_UNFINISHED:
            if(c != '=') {
                REWIND_CHAR(c);
                return LEX_FSM__ADD;
            }

            return LEX_FSM__ASSIGN_ADD;
            // String states

        case LEX_FSM__SUBTRACT_UNFINISHED:
            if(c != '=') {
                REWIND_CHAR(c);
                return LEX_FSM__SUBTRACT;
            }

            return LEX_FSM__ASSIGN_SUB;

        case LEX_FSM__MULTIPLY_UNFINISHED:
            if(c != '=') {
                REWIND_CHAR(c);
                return LEX_FSM__MULTIPLY;
            }

            return LEX_FSM__ASSIGN_MULTIPLY;

        case LEX_FSM__INTEGER_DIVIDE_UNFINISHED:
            if(c != '=') {
                REWIND_CHAR(c);
                return LEX_FSM__INTEGER_DIVIDE;
            }

            return LEX_FSM__ASSIGN_INT_DIVIDE;

            // Strings

        case LEX_FSM__STRING_EXC:
            if(c == '"') {
                return LEX_FSM__STRING_LOAD;
            } else {
                lexer_fsm->lexer_error = LEXER_ERROR__STRING_FORMAT;
                return LEX_FSM__ERROR;
            }

        case LEX_FSM__STRING_LOAD:

            if(c < 32 || c == EOF) {
                lexer_fsm->lexer_error = LEXER_ERROR__STRING_FORMAT;
                return LEX_FSM__ERROR;
            }
            switch(c) {
                case '"':
                    return LEX_FSM__STRING_VALUE;
                case '\\':
                    return LEX_FSM__STRING_SLASH;
                default:
                    STORE_CHAR(c);
                    return LEX_FSM__STRING_LOAD;
            }

        case LEX_FSM__STRING_SLASH:
            if(isdigit(c)) {
                lexer_fsm->numeric_char_value[lexer_fsm->numeric_char_position = 0] = (char) c;
                return LEX_FSM__STRING_NUMERIC_CHAR;
            }

            switch(c) {
                case '\"':
                    STORE_CHAR('"');
                    return LEX_FSM__STRING_LOAD;
                case '\\':
                    STORE_CHAR('\\');
                    return LEX_FSM__STRING_LOAD;
                case 'n':
                    STORE_CHAR('\n');
                    return LEX_FSM__STRING_LOAD;
                case 't':
                    STORE_CHAR('\t');
                    return LEX_FSM__STRING_LOAD;
                default:
                    lexer_fsm->lexer_error = LEXER_ERROR__STRING_FORMAT;
                    return LEX_FSM__ERROR;
            }

            // Integer literal
        case LEX_FSM__STRING_NUMERIC_CHAR:
            if(isdigit(c)) {
                lexer_fsm->numeric_char_value[++lexer_fsm->numeric_char_position] = (char) c;
                if(lexer_fsm->numeric_char_position == 2) {

                    lexer_fsm->numeric_char_value[3] = '\0';
                    int numeric_char_value = atoi(lexer_fsm->numeric_char_value);

                    if(numeric_char_value > 0 && numeric_char_value <= 255) {
                        string_append_c(lexer_fsm->stream_buffer, (char) numeric_char_value);
                        return LEX_FSM__STRING_LOAD;
                    }
                    // not in escape range
                    lexer_fsm->lexer_error = LEXER_ERROR__STRING_FORMAT;
                    return LEX_FSM__ERROR;

                }
                return LEX_FSM__STRING_NUMERIC_CHAR;

            }
            // invalid char in \ddd sequence
            lexer_fsm->lexer_error = LEXER_ERROR__STRING_FORMAT;
            return LEX_FSM__ERROR;

        case LEX_FSM__INTEGER_LITERAL_UNFINISHED:
            if(isdigit(c)) {
                STORE_CHAR(c);
                return LEX_FSM__INTEGER_LITERAL_UNFINISHED;
            } else if(c == '.') {
                STORE_CHAR(c);
                return LEX_FSM__DOUBLE_DOT;
            } else if(tolower(c) == 'e') {
                STORE_CHAR(c);
                return LEX_FSM__DOUBLE_E;
            } else {
                REWIND_CHAR(c);
                return LEX_FSM__INTEGER_LITERAL_FINISHED;
            }
            // Double literal

        case LEX_FSM__DOUBLE_DOT:
            if(isdigit(c)) {
                STORE_CHAR(c);
                return LEX_FSM__DOUBLE_UNFINISHED;
            } else {
                lexer_fsm->lexer_error = LEXER_ERROR__DOUBLE_FORMAT;
                return LEX_FSM__ERROR;
            }

        case LEX_FSM__DOUBLE_UNFINISHED:
            if(isdigit(c)) {
                STORE_CHAR(c);
                return LEX_FSM__DOUBLE_UNFINISHED;
            } else if(tolower(c) == 'e') {
                STORE_CHAR(c);
                return LEX_FSM__DOUBLE_E;
            } else {
                REWIND_CHAR(c);
                return LEX_FSM__DOUBLE_FINISHED;
            }

        case LEX_FSM__DOUBLE_E:
            if(isdigit(c)) {
                STORE_CHAR(c);
                return LEX_FSM__DOUBLE_E_UNFINISHED;
            } else if(c == '-' || c == '+') {
                STORE_CHAR(c);
                return LEX_FSM__DOUBLE_E_SIGN;
            } else {
                lexer_fsm->lexer_error = LEXER_ERROR__DOUBLE_FORMAT;
                return LEX_FSM__ERROR;
            }

        case LEX_FSM__DOUBLE_E_SIGN:
            if(isdigit(c)) {
                STORE_CHAR(c);
                return LEX_FSM__DOUBLE_E_UNFINISHED;
            } else {
                lexer_fsm->lexer_error = LEXER_ERROR__DOUBLE_FORMAT;
                return LEX_FSM__ERROR;
            }

        case LEX_FSM__DOUBLE_E_UNFINISHED:
            if(isdigit(c)) {
                STORE_CHAR(c);
                return LEX_FSM__DOUBLE_E_UNFINISHED;
            } else {
                REWIND_CHAR(c);
                return LEX_FSM__DOUBLE_FINISHED;
            }

            // Relation operators
        case LEX_FSM__LEFT_SHARP_BRACKET:
            switch(c) {
                case '=':
                    return LEX_FSM__SMALLER_EQUAL;
                case '>':
                    return LEX_FSM__SMALLER_BIGGER;
                default:
                    REWIND_CHAR(c);
                    return LEX_FSM__SMALLER;
            }

        case LEX_FSM__RIGHT_SHARP_BRACKET:

            if(c == '=')
                return LEX_FSM__BIGGER_EQUAL;
            else {
                REWIND_CHAR(c);
                return LEX_FSM__BIGGER;
            }

            // Identifiers
        case LEX_FSM__IDENTIFIER_UNFINISHED:
            if(c == '_' || isalnum(c)) {
                STORE_CHAR(tolower(c));
                return LEX_FSM__IDENTIFIER_UNFINISHED;
            } else {
                REWIND_CHAR(tolower(c));
                LexerFSMState return_state = lexer_fsm_get_identifier_state(string_content(lexer_fsm->stream_buffer));
                return return_state;
            }

            // Comments
        case LEX_FSM__SLASH:
            if(c == '=')
                return LEX_FSM__ASSIGN_DIVIDE;
            if(c == '\'')
                return LEX_FSM__COMMENT_BLOCK;
            else {

                REWIND_CHAR(c);
                return LEX_FSM__DIVIDE;
            }

        case LEX_FSM__COMMENT_LINE:
            if(c != '\n' && c != EOF)
                return LEX_FSM__COMMENT_LINE;
            REWIND_CHAR(tolower(c));
            return LEX_FSM__INIT;

        case LEX_FSM__COMMENT_BLOCK:
            if(c == EOF)
                return LEX_FSM__ERROR;
            if(c == '\'')
                return LEX_FSM__COMMENT_BLOCK_END;
            return LEX_FSM__COMMENT_BLOCK;

        case LEX_FSM__COMMENT_BLOCK_END:
            if(c == '/')
                return LEX_FSM__INIT;
            else
                return LEX_FSM__COMMENT_BLOCK;

        default:
            break;
    }

    lexer_fsm->lexer_error = LEXER_ERROR__ERROR_LEXEM;
    return LEX_FSM__ERROR;
}

LexerFSMState lexer_fsm_get_identifier_state(const char* name) {

    static const char* keywords[] = {
            // keywords
            "as", "asc", "declare", "dim", "do",
            "else", "end", "chr", "function",
            "if", "input", "length", "loop",
            "print", "return", "scope", "substr",
            "then", "while",
            // reserved
            "and", "continue",
            "elseif", "exit", "false", "for", "next", "not",
            "or", "shared", "static", "true",
            // data type keywords
            "integer", "double", "boolean", "string"
    };

    static const int number_of_keywords = sizeof(keywords) / sizeof(*keywords);

    for(int i = 0; i < number_of_keywords; i++) {
        if(strcmp(keywords[i], name) == 0) {
            if(i >= 31)
                return LEX_FSM__INTEGER + (i - 31);
            return LEX_FSM__AS + i;
        }
    }

    return LEX_FSM__IDENTIFIER_FINISHED;
}

