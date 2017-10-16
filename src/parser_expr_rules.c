#include "parser_expr_rules.h"

const expression_rule_function expr_rule_table[EXPR_RULE_TABLE_SIZE] = {
        expression_rule_id,
        expression_rule_fn,
        expression_rule_add,
        expression_rule_fake
};

bool expression_rule_example(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
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
    (void) expr_id;

    ExprToken* e = create_expression((*expression_idx)++);

    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_fake(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    // ***FAKE Reduction***
    ExprToken* tmp;
    ExprTokenType type;
    do {
        tmp = llist_pop_back(expr_token_buffer);
        if(tmp == NULL) {
            return false;
        }
        type = tmp->type;
        expr_token_free(tmp);
    } while(type != EXPR_LEFT_SHARP);
    llist_append(expr_token_buffer, create_expression((*expression_idx)++));
    return true;
}

// --------------- ACTUAL RULES ---------------
bool expression_rule_id(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> i
    */

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    if(it->previous != NULL) { it = it->previous; } else { return false; }
    if(it != NULL && it->value != NULL) {
        ExprTokenType tt = ((ExprToken*) it->value)->type;
        if(tt != EXPR_TOKEN_IDENTIFIER &&
           tt != EXPR_TOKEN_BOOLEAN_LITERAL &&
           tt != EXPR_TOKEN_INTEGER_LITERAL &&
           tt != EXPR_TOKEN_DOUBLE_LITERAL &&
           tt != EXPR_TOKEN_STRING_LITERAL) {
            return false;
        }
    }
    EXPR_RULE_CHECK_FINISH();

    // NOTE: now we are processing rule regular way - from the left to the right
    ExprToken* i = (ExprToken*) tmp->next->value;
    (void) i->type;
    (void) i->data.s;
    if(i->type == EXPR_TOKEN_INTEGER_LITERAL) {
        CodeConstructor* constructor = parser->code_constructor;
        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_integer(atoi(i->data.s)));
    } else if(i->type == EXPR_TOKEN_DOUBLE_LITERAL) {
        CodeConstructor* constructor = parser->code_constructor;
        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_double(atof(i->data.s)));
    } else if(i->type == EXPR_TOKEN_STRING_LITERAL) {
        CodeConstructor* constructor = parser->code_constructor;
        String* string = string_init();
        string_append_s(string, i->data.s);
        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_string(string));
        string_free(&string);
    } else if(i->type == EXPR_TOKEN_BOOLEAN_LITERAL) {
        CodeConstructor* constructor = parser->code_constructor;
        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_boolean(0 == strcmp(i->data.s, "true")));
    }

    ExprToken* e = create_expression((*expression_idx)++);

    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_fn(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE  // Note: fn token includes right bracket token
    * E -> fn )
    * E -> fn E )
    * E -> fn E, E )
    * E -> fn E, E, ... )
    */

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_RIGHT_BRACKET);

    ExprTokenType tt;
    unsigned int arg_count = 0;

    if(it->previous != NULL) {
        it = it->previous;
        tt = ((ExprToken*) it->value)->type;
    } else { return false; }
    if(it->value != NULL) {
        if(tt != EXPR_TOKEN_FUNCTION_CALL &&
           tt != EXPR_EXPRESSION) { return false; }
        if(tt == EXPR_EXPRESSION) { arg_count++; }
    } else { return false; }

    // arg_count in while condition is only for entering infinite while, if arg_count > 0
    while(arg_count) {
        if(it->previous != NULL) {
            it = it->previous;
            tt = ((ExprToken*) it->value)->type;
        } else { return false; }
        if(it->value != NULL) {
            if(tt == EXPR_TOKEN_FUNCTION_CALL) { break; }
            if(tt != EXPR_TOKEN_COMMA) { return false; }
        } else { return false; }

        if(it->previous != NULL) {
            it = it->previous;
            tt = ((ExprToken*) it->value)->type;
        } else { return false; }
        if(it->value != NULL) {
            if(tt == EXPR_EXPRESSION) {
                arg_count++;
            } else {
                return false;
            }
        } else { return false; }
    }

    EXPR_RULE_CHECK_FINISH();

    // NOTE: now we are processing rule regular way - from the left to the right

    char* function_name = ((ExprToken*) tmp->next->value)->data.s;
    UNUSED(function_name);

    for(unsigned int i = 0; i < arg_count; i++) {
        ExprIdx idx = EXPR_RULE_NEXT_E_ID();
        (void) idx;
    }

    ExprToken* e = create_expression((*expression_idx)++);

    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_add(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
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

    (void) a;
    (void) b;

    CodeConstructor* constructor = parser->code_constructor;
    GENERATE_CODE(I_ADD_STACK);

    ExprToken* e = create_expression((*expression_idx)++);

    EXPR_RULE_REPLACE(e);

    return true;
}