#ifndef _ERROR_H
#define _ERROR_H

typedef enum error_code_t {
    ERROR_NONE = 0,
    ERROR_LEXER = 1,
    ERROR_SYNTAX = 2,
    ERROR_SEMANTIC_DEFINITION = 3,
    ERROR_SEMANTIC_TYPE = 4,
    ERROR_SEMANTIC_OTHER = 6,
    ERROR_INTERNAL = 99,
    ERROR_MEMORY = 99,
} ErrorCode;

void exit_with_code(ErrorCode code);

#endif //_ERROR_H
