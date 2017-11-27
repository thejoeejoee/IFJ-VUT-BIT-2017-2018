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
            fprintf(stderr, "Error during semantic definition.\n");
            break;
        case ERROR_SEMANTIC_TYPE:
            fprintf(stderr, "Error in type definition.\n");
            break;
        case ERROR_SEMANTIC_OTHER:
            fprintf(stderr, "Error unknown semantic.\n");
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
            fprintf(stderr, "Error during lexical analyse. On the line %lu, ", (long unsigned) error_report.line);

            switch(error_report.detail_information) {
                case LEXER_ERROR__STRING_FORMAT:
                    fprintf(stderr, "wrong string format!");
                    break;
                case LEXER_ERROR__DOUBLE_FORMAT:
                    fprintf(stderr, "wrong double format!");
                    break;
                case LEXER_ERROR__ERROR_LEXEM:
                    fprintf(stderr, "unexpected lexical unit!");
                    break;
                default:
                    fprintf(stderr, "unknown problem.");
            }

            break;
        case ERROR_SYNTAX:
            fprintf(stderr, "Error during syntax analyse on the line %lu.", (long unsigned) error_report.line);
            break;
        case ERROR_SEMANTIC_DEFINITION:
            fprintf(stderr, "Error in semantic definition on the line %lu.", (long unsigned) error_report.line);
            break;
        case ERROR_SEMANTIC_TYPE:
            fprintf(stderr, "Error semantic type definition on the line %lu.", (long unsigned) error_report.line);
            break;
        case ERROR_SEMANTIC_OTHER:
            fprintf(stderr, "Error in semantic on the line %lu.", (long unsigned) error_report.line);
            break;
        case ERROR_INTERNAL:
            fprintf(stderr, "Internal compiler error on the line %lu.", (long unsigned) error_report.line);
            break;
        default:
            break;
    }

    fprintf(stderr, "\n");

    // free all allocated memory blocks
    memory_manager_exit(&memory_manager);
    exit(error_report.error_code);
}
