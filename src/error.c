#include "error.h"
#include "debug.h"

short exit_error_code = ERROR_NONE;

void exit_with_code(ErrorCode code) {
    switch (code) {
        case ERROR_LEXER:
            fprintf(stderr, "Error during lexical analyse.\n");
            break;
        case ERROR_SYNTAX:
            fprintf(stderr, "Error during syntax analyse.\n");
            break;
        case ERROR_SEMANTIC_DEFINITION:
            fprintf(stderr, "Error in semantic definitions.\n");
            break;
        case ERROR_SEMANTIC_TYPE:
            fprintf(stderr, "Error semantic type definitions.\n");
            break;
        case ERROR_SEMANTIC_OTHER:
            fprintf(stderr, "Error in semantic.\n");
            break;
        case ERROR_INTERNAL:
            fprintf(stderr, "Internal compiler error.\n");
            break;
        default:;
    }
    exit(code);
}
