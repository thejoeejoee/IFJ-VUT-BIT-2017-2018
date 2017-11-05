#ifndef _PARSER_EXPR_RULES_H
#define _PARSER_EXPR_RULES_H

#include "parser.h"
#include "parser_expr_internal.h"
#include "llist.h"

// Helper macros

#define EXPR_RULE_CHECK_START() LListBaseItem *tmp = NULL, *it = expr_token_buffer->tail
#define EXPR_RULE_CHECK_TYPE(expr_token_type) do {\
    if (it->previous != NULL) { it = it->previous; } else { return false; }\
    if (!(it != NULL && ((ExprToken*)it)->type == (expr_token_type))) { return false; }\
    UNUSED(tmp); \
    UNUSED(it); \
} while(false)
#define EXPR_RULE_CHECK_FINISH() EXPR_RULE_CHECK_TYPE(EXPR_LEFT_SHARP); tmp = it;
#define EXPR_RULE_NEXT_E_ID() get_next_expr(&tmp)->data.idx
#define EXPR_RULE_NEXT_E() get_next_expr(&tmp)
#define EXPR_RULE_NEXT_E_ITEM() get_next_expr_item(&tmp)
#define EXPR_RULE_REPLACE(single_expression) expr_replace(expr_token_buffer, it, (single_expression))
#define EXPR_LOWER_OPERAND get_n_expr(expr_token_buffer, 3)
#define EXPR_HIGHER_OPERAND get_n_expr(expr_token_buffer, 1)
#define EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(operation) SEMANTIC_ANALYSIS({ \
        const DataType target_type = parser_semantic_resolve_implicit_data_type_conversion( \
        parser->parser_semantic, \
        operation, EXPR_LOWER_OPERAND->data_type, EXPR_HIGHER_OPERAND->data_type); \
        if(target_type == DATA_TYPE_NONE) {\
            parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_TYPE; \
            return false; \
        } \
    })

#define EXPR_CHECK_UNARY_OPERATION_IMPLICIT_CONVERSION(operation) SEMANTIC_ANALYSIS({ \
        const DataType target_type = parser_semantic_resolve_implicit_data_type_conversion( \
        parser->parser_semantic, \
        operation, DATA_TYPE_NONE, EXPR_HIGHER_OPERAND->data_type); \
        if(target_type == DATA_TYPE_NONE) {\
            parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_TYPE; \
            return false; \
        } \
    })

#define EXPR_CHECK_UNARY_OPERATION_IMPLICIT_CONVERSION_FROM_DATA_TYPE(operation, source_data_type) SEMANTIC_ANALYSIS({ \
        const DataType target_type = parser_semantic_resolve_implicit_data_type_conversion( \
        parser->parser_semantic, \
        operation, DATA_TYPE_NONE, source_data_type); \
        if(target_type == DATA_TYPE_NONE) {\
            parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_TYPE; \
            return false; \
        } \
    })


#define CREATE_EXPR_RESULT_OF_BINARY_OPERATION(operation) \
    ExprToken* e = create_expression((*expression_idx)++); \
    const OperationSignature* operation_signature = NULL; \
    SEMANTIC_ANALYSIS({ \
        operation_signature = parser_semantic_get_operation_signature( \
                parser->parser_semantic, \
                (operation), \
                EXPR_LOWER_OPERAND->data_type, \
                EXPR_HIGHER_OPERAND->data_type \
        ); \
        if (operation_signature != NULL)    \
            e->data_type = operation_signature->result_type; \
    }); \

#define GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE() do {\
        if(operation_signature != NULL) \
            GENERATE_STACK_DATA_TYPE_CONVERSION_CODE( \
                    EXPR_LOWER_OPERAND->data_type, \
                    EXPR_HIGHER_OPERAND->data_type, \
                    operation_signature->conversion_target_type \
            ); \
    } while(0)



// --------------------------

#define EXPR_RULE_TABLE_SIZE 19

typedef bool(*expression_rule_function)(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
extern const expression_rule_function expr_rule_table[EXPR_RULE_TABLE_SIZE];

// Expression rule headers
bool expression_rule_fake(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_example(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);

// Actual Rules
bool expression_rule_id(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_brackets(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_fn(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);

bool expression_rule_fn_length(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_fn_substr(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_fn_asc(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_fn_chr(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);

bool expression_rule_add(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_sub(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);

bool expression_rule_mul(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);

bool expression_rule_div(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);

bool expression_rule_div_int(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);

bool expression_rule_unary_minus(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);

// Comparison
bool expression_rule_greater(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_greater_or_equal(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_equal(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_not_equal(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_lesser_or_equal(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_lesser(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);

#endif //_PARSER_EXPR_RULES_H
