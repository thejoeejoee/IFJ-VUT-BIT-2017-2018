#include "parser_expr_rules.h"

const expression_rule_function expr_rule_table[EXPR_RULE_TABLE_SIZE] = {
    expression_rule_add,
    expression_rule_fake
};

bool expression_rule_example(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E
    */

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();

    // NOTE: now we are processing rule regular way - from the left to the right
    ExprIdx expr_id = EXPR_RULE_NEXT_E_ID();
    (void)expr_id;

    ExprToken* e = create_expression((*expression_idx)++);

    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_fake(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx) {
    // ***FAKE Reduction***
    ExprToken *tmp;
    ExprTokenType type;
    do {
        tmp = llist_pop_back(expr_token_buffer);
        if (tmp == NULL) {
            return false;
        }
        type = tmp->type;
        expr_token_free(tmp);
    } while (type != EXPR_LEFT_SHARP);
    return true;
}

// --------------- ACTUAL RULES ---------------
bool expression_rule_add(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E + E
    */

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_PLUS);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();

    // NOTE: now we are processing rule regular way - from the left to the right
    ExprIdx a = EXPR_RULE_NEXT_E_ID();
    ExprIdx b = EXPR_RULE_NEXT_E_ID();
    
    (void)a; (void)b;

    ExprToken* e = create_expression((*expression_idx)++);

    EXPR_RULE_REPLACE(e);

    return true;
}