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
#define EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(operation) do { \
        const DataType target_type = parser_semantic_resolve_implicit_data_type_conversion( \
        parser->parser_semantic, \
        operation, EXPR_LOWER_OPERAND->data_type, EXPR_HIGHER_OPERAND->data_type); \
        if(target_type == DATA_TYPE_NONE) \
            return false; \
    } while(false)


// --------------------------

#define EXPR_RULE_TABLE_SIZE 9

typedef bool(*expression_rule_function)(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
extern const expression_rule_function expr_rule_table[EXPR_RULE_TABLE_SIZE];

// Expression rule headers
bool expression_rule_fake(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_example(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);

// Actual Rules
bool expression_rule_id(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_fn(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_add(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);

bool expression_rule_sub(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);

bool expression_rule_unary_minus(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);

// Boolshit
bool expression_rule_greater(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);
bool expression_rule_greater_or_equal(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx);

bool expression_rule_equal(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx);

#endif //_PARSER_EXPR_RULES_H
