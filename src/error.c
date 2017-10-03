#include "error.h"
#include "debug.h"
#include "memory.h"
#include "lexer_fsm.h"

void exit_with_code(ErrorCode code) {
    switch(code) {
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
    // free all allocated memory blocksor
    memory_manager_exit(&memory_manager);
    exit(code);
}

void exit_with_detail_information(ErrorReport error_report) {
    switch(error_report.error_code) {
        case ERROR_LEXER:
            fprintf(stderr, "Error during lexical analyse. On the line %d, ", error_report.line);

            switch (error_report.detail_information) {
                case LEXER_ERROR__STRING_FORMAT:
                    fprintf(stderr, "error string format!");
                    break;
                case LEXER_ERROR__DOUBLE_FORMAT:
                    fprintf(stderr, "error double format!");
                    break;
                case LEXER_ERROR__ERROR_LEXEM:
                    fprintf(stderr, "unexpected lexical unit!");
                    break;
            }

            break;
        case ERROR_SYNTAX:
            fprintf(stderr, "Error during syntax analyse. On the line %d,", error_report.line);
            break;
        case ERROR_SEMANTIC_DEFINITION:
            fprintf(stderr, "Error in semantic definitions. On the line %d,", error_report.line);
            break;
        case ERROR_SEMANTIC_TYPE:
            fprintf(stderr, "Error semantic type definitions. On the line %d,", error_report.line);
            break;
        case ERROR_SEMANTIC_OTHER:
            fprintf(stderr, "Error in semantic. On the line %d,", error_report.line);
            break;
        case ERROR_INTERNAL:
            fprintf(stderr, "Internal compiler error. On the line %d,", error_report.line);
            break;
        default: break;

    }

    fprintf(stderr, "\n");

    // free all allocated memory blocks
    memory_manager_exit(&memory_manager);
    exit(error_report.error_code);
}
