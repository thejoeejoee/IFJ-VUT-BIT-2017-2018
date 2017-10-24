#include "parser_expr_rules.h"

const expression_rule_function expr_rule_table[EXPR_RULE_TABLE_SIZE] = {
        expression_rule_id,
        expression_rule_fn,
        expression_rule_add,
        expression_rule_sub,
        expression_rule_unary_minus,
        expression_rule_greater,
        expression_rule_greater_or_equal,
        expression_rule_fake
};

bool expression_rule_example(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E
    */
    UNUSED(parser);

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
    UNUSED(parser);

    ExprToken* tmp;
    ExprTokenType type;
    DataType expr_data_type;

    do {
        tmp = (ExprToken*) llist_pop_back(expr_token_buffer);
        if(tmp == NULL) {
            return false;
        }
        type = tmp->type;
        if(tmp->type == EXPR_EXPRESSION)
            expr_data_type = tmp->data_type;
        expr_token_free(tmp);
    } while(type != EXPR_LEFT_SHARP);

    ExprToken* new_token = create_expression((*expression_idx)++);
    new_token->data_type = expr_data_type;

    llist_append_item(expr_token_buffer, (LListBaseItem*)new_token);

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
    if(it != NULL) {
        ExprTokenType tt = ((ExprToken*) it)->type;
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
    ExprToken* i = (ExprToken*) tmp->next;
    (void) i->type;
    (void) i->data.s;
    i->data_type = DATA_TYPE_NONE;
    if(i->type == EXPR_TOKEN_INTEGER_LITERAL) {
        i->data_type = DATA_TYPE_INTEGER;

        CodeConstructor* constructor = parser->code_constructor;
        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_integer(atoi(i->data.s)));

    } else if(i->type == EXPR_TOKEN_DOUBLE_LITERAL) {
        i->data_type = DATA_TYPE_DOUBLE;

        CodeConstructor* constructor = parser->code_constructor;
        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_double(atof(i->data.s)));

    } else if(i->type == EXPR_TOKEN_STRING_LITERAL) {
        i->data_type = DATA_TYPE_STRING;

        CodeConstructor* constructor = parser->code_constructor;
        String* string = string_init();
        string_append_s(string, i->data.s);
        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_string(string));
        string_free(&string);

    } else if(i->type == EXPR_TOKEN_BOOLEAN_LITERAL) {
        i->data_type = DATA_TYPE_BOOLEAN;

        CodeConstructor* constructor = parser->code_constructor;
        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_boolean(0 == strcmp(i->data.s, "true")));

    } else if(i->type == EXPR_TOKEN_IDENTIFIER) {
        SymbolVariable* variable = symbol_register_find_variable_recursive(
                parser->parser_semantic->register_,
                i->data.s
        );

        if(variable == NULL) {
            parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
            return false;
        }
        i->data_type = variable->data_type;

        CodeConstructor* constructor = parser->code_constructor;
        GENERATE_CODE(
                I_PUSH_STACK,
                code_instruction_operand_init_variable(variable)
        );
    }

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = i->data_type;

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
    CodeConstructor* constructor = parser->code_constructor;
    UNUSED(parser);

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_RIGHT_BRACKET);

    ExprTokenType tt;
    unsigned int arg_count = 0;

    if(it->previous != NULL) {
        it = it->previous;
        tt = ((ExprToken*) it)->type;
    } else { return false; }
    if(tt != EXPR_TOKEN_LEFT_BRACKET &&
       tt != EXPR_EXPRESSION) { return false; }
    if(tt == EXPR_EXPRESSION) { arg_count++; }

    // arg_count in while condition is only for entering infinite while, if arg_count > 0
    while(arg_count) {
        if(it->previous != NULL) {
            it = it->previous;
            tt = ((ExprToken*) it)->type;
        } else { return false; }
        if(tt == EXPR_TOKEN_LEFT_BRACKET) { break; }
        if(tt != EXPR_TOKEN_COMMA) { return false; }

        if(it->previous != NULL) {
            it = it->previous;
            tt = ((ExprToken*) it)->type;
        } else { return false; }
        if(tt == EXPR_EXPRESSION) {
            arg_count++;
        } else {
            return false;
        }
    }

    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_IDENTIFIER);
    EXPR_RULE_CHECK_FINISH();

    // NOTE: now we are processing rule regular way - from the left to the right

    char* function_name = ((ExprToken*) tmp->next)->data.s;
    SymbolFunction* function = symbol_table_function_get(
            parser->parser_semantic->register_->functions,
            function_name
    );
    if(function == NULL) {
        parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
        return false;
    }
    String* function_label = symbol_function_generate_function_label(function);
    GENERATE_CODE(
            I_CALL,
            code_instruction_operand_init_label(string_content(function_label))
    );
    string_free(&function_label);

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
    // backward
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_PLUS);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();

    const DataType operand_1_type = EXPR_LOWER_OPERAND->data_type;
    const DataType operand_2_type = EXPR_HIGHER_OPERAND->data_type;
    const DataType result_type = parser_semantic_resolve_implicit_data_type_conversion(
            parser->parser_semantic,
            OPERATION_ADD, operand_1_type, operand_2_type);
    if(result_type == DATA_TYPE_NONE)
        return false;

    CodeConstructor* constructor = parser->code_constructor;
    // generate conversion
    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(operand_1_type, operand_2_type, result_type);
    GENERATE_CODE(I_ADD_STACK);

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = result_type;

    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_sub(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E + E
    */
    // backward
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_MINUS);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();

    const DataType operand_1_type = EXPR_LOWER_OPERAND->data_type;
    const DataType operand_2_type = EXPR_HIGHER_OPERAND->data_type;
    const DataType result_type = parser_semantic_resolve_implicit_data_type_conversion(
            parser->parser_semantic,
            OPERATION_SUB, operand_1_type, operand_2_type);
    if(result_type == DATA_TYPE_NONE)
        return false;

    CodeConstructor* constructor = parser->code_constructor;
    // generate conversion
    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(operand_1_type, operand_2_type, result_type);
    GENERATE_CODE(I_SUB_STACK);

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = result_type;

    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_unary_minus(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> - E
    */
    // backward
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_UNARY_MINUS);
    EXPR_RULE_CHECK_FINISH();

    // TODO add type cast
    CodeConstructor* constructor = parser->code_constructor;
    GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_integer(-1));
    GENERATE_CODE(I_MUL_STACK);

    ExprToken* e = create_expression((*expression_idx)++);
    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_greater(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
     * RULE
     * E -> E > E
     */

    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_GREATHER);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();

    const DataType operand_1_type = EXPR_LOWER_OPERAND->data_type;
    const DataType operand_2_type = EXPR_HIGHER_OPERAND->data_type;
    const DataType result_type = parser_semantic_resolve_implicit_data_type_conversion(
            parser->parser_semantic,
            OPERATION_GREATER, operand_1_type, operand_2_type);
    if(result_type == DATA_TYPE_NONE)
        return false;

    CodeConstructor* constructor = parser->code_constructor;
    // generate conversion
    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(operand_1_type, operand_2_type, result_type);
    GENERATE_CODE(I_GREATER_THEN_STACK);

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = DATA_TYPE_BOOLEAN;
    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_greater_or_equal(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
     * RULE
     * E -> E >= E
     */

    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_GREATHER_OR_EQUAL);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();

    const DataType operand_1_type = EXPR_LOWER_OPERAND->data_type;
    const DataType operand_2_type = EXPR_HIGHER_OPERAND->data_type;
    const DataType result_type = parser_semantic_resolve_implicit_data_type_conversion(
            parser->parser_semantic,
            OPERATION_GREATER_OR_EQUAL, operand_1_type, operand_2_type);
    if(result_type == DATA_TYPE_NONE)
        return false;

    CodeConstructor* constructor = parser->code_constructor;
    // generate conversion
    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(operand_1_type, operand_2_type, result_type);

    GENERATE_CODE(I_LESS_THEN_STACK);
    GENERATE_CODE(I_NOT_STACK);

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = DATA_TYPE_BOOLEAN;
    EXPR_RULE_REPLACE(e);

    return true;
}
