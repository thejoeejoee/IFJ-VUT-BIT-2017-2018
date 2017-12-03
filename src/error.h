#ifndef _ERROR_H
#define _ERROR_H

#include "stdlib.h"

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

typedef struct error_report_t {
    ErrorCode error_code;
    size_t line;
    short detail_information;
} ErrorReport;

/**
 * Exit program with given code. It also free all allocated memory blocks.
 * @param code exit code
 */
void exit_with_code(ErrorCode code);

/**
 * Exit program with given error report - info and error code. It also free all allocated memory blocks.
 * @param code exit code
 */
void exit_with_detail_information(ErrorReport error_report);

#endif //_ERROR_H
