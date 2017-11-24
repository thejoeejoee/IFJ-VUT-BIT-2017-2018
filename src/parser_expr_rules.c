#include "parser_expr_rules.h"
#include "stdlib.h"
#include "code_instruction_operand.h"
#include "code_optimizer_expr.h"

const expression_rule_function expr_rule_table[EXPR_RULE_TABLE_SIZE] = {
        expression_rule_id,
        expression_rule_fn,
        expression_rule_fn_length,
        expression_rule_fn_substr,
        expression_rule_fn_asc,
        expression_rule_fn_chr,
        expression_rule_brackets,
        expression_rule_add,
        expression_rule_sub,
        expression_rule_mul,
        expression_rule_div,
        expression_rule_div_int,
        expression_rule_unary_minus,
        expression_rule_not,
        expression_rule_and,
        expression_rule_or,
        expression_rule_greater,
        expression_rule_greater_or_equal,
        expression_rule_equal,
        expression_rule_not_equal,
        expression_rule_lesser_or_equal,
        expression_rule_lesser
};

bool expression_rule_fake(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    // ***FAKE Reduction***
    UNUSED(parser);

    ExprToken* tmp;
    ExprTokenType type;
    DataType expr_data_type = DATA_TYPE_NONE;

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

    llist_append_item(expr_token_buffer, (LListBaseItem*) new_token);

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

    ExprToken* e = create_expression((*expression_idx)++);

    CodeConstructor* constructor = parser->code_constructor;
    if(i->type == EXPR_TOKEN_INTEGER_LITERAL) {
        e->data_type = DATA_TYPE_INTEGER;
        e->is_constant = true;
        e->instruction = GENERATE_CODE(I_PUSH_STACK,
                                       code_instruction_operand_init_integer(strtol(i->data.s, NULL, 10)));

    } else if(i->type == EXPR_TOKEN_DOUBLE_LITERAL) {
        e->data_type = DATA_TYPE_DOUBLE;
        e->is_constant = true;
        e->instruction = GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_double(strtod(i->data.s, NULL)));

    } else if(i->type == EXPR_TOKEN_STRING_LITERAL) {
        e->data_type = DATA_TYPE_STRING;
        e->is_constant = true;
        String* string = string_init();
        string_append_s(string, i->data.s);
        e->instruction = GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_string(string));
        string_free(&string);

    } else if(i->type == EXPR_TOKEN_BOOLEAN_LITERAL) {
        e->data_type = DATA_TYPE_BOOLEAN;
        e->is_constant = true;

        GENERATE_CODE(I_PUSH_STACK, code_instruction_operand_init_boolean(i->data.b));

    } else if(i->type == EXPR_TOKEN_IDENTIFIER) {
        SymbolVariable* variable = symbol_register_find_variable_recursive(
                parser->parser_semantic->register_,
                i->data.s
        );
        SEMANTIC_ANALYSIS(
                {

                    if(variable == NULL) {
                        parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
                        return false;
                    }

                    e->data_type = variable->data_type;
                }
        );

        CODE_GENERATION(
                {
                    GENERATE_CODE(
                            I_PUSH_STACK,
                            code_instruction_operand_init_variable(variable)
                    );
                }
        );
    }

    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_brackets(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    UNUSED(parser);

    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_RIGHT_BRACKET);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_LEFT_BRACKET);
    EXPR_RULE_CHECK_FINISH();

    ExprToken* e = create_expression((*expression_idx)++);
    // 2 because 0 is right sharp and 1 is bracket
    ExprToken* content = get_n_expr(expr_token_buffer, 2);

    e->is_constant = content->is_constant;
    e->instruction = content->instruction;
    e->data_type = content->data_type;
    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_fn(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> id ( )
    * E -> id ( E )
    * E -> id ( E, E )
    * E -> id ( E, E, ... )
    */
    CodeConstructor* constructor = parser->code_constructor;
    UNUSED(parser);

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_RIGHT_BRACKET);

    ExprTokenType tt;
    size_t arg_count = 0;

    if(it->previous != NULL) {
        it = it->previous;
        tt = ((ExprToken*) it)->type;
    } else {
        return false;
    }
    if(tt != EXPR_TOKEN_LEFT_BRACKET &&
       tt != EXPR_EXPRESSION) {
        return false;
    }
    if(tt == EXPR_EXPRESSION) {
        arg_count++;
    }

    // arg_count in while condition is only for entering infinite while, if arg_count > 0
    while(arg_count) {
        if(it->previous != NULL) {
            it = it->previous;
            tt = ((ExprToken*) it)->type;
        } else {
            return false;
        }
        if(tt == EXPR_TOKEN_LEFT_BRACKET) {
            break;
        }
        if(tt != EXPR_TOKEN_COMMA) {
            return false;
        }

        if(it->previous != NULL) {
            it = it->previous;
            tt = ((ExprToken*) it)->type;
        } else {
            return false;
        }
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
    SEMANTIC_ANALYSIS(
            {
                if(function == NULL) {
                    parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
                    return false;
                }
            }
    );

    GENERATE_CODE(I_CREATE_FRAME);
    SymbolFunctionParam* param = NULL;
    SEMANTIC_ANALYSIS(
            {
                param = function->param_tail;
                for(size_t i = 1; i <= arg_count; i++) {
                    ExprToken* token = get_n_expr(expr_token_buffer, 2 * i);

                    if(param == NULL) {
                        parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_TYPE;
                        return false;

                    }
                    // check valid implicit conversion on stack top to token->data_type
                    EXPR_CHECK_UNARY_OPERATION_IMPLICIT_CONVERSION_FROM_DATA_TYPE(
                            OPERATION_IMPLICIT_CONVERSION,
                            token->data_type,
                            param->data_type
                    );

                    // generate implicit conversion
                    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(
                            token->data_type,
                            param->data_type
                    );
                    // declare local variable on TF
                    GENERATE_CODE(
                            I_DEF_VAR,
                            code_instruction_operand_init_variable_from_param(function, param)
                    );
                    // fill with (already converted) value from stack
                    GENERATE_CODE(
                            I_POP_STACK,
                            code_instruction_operand_init_variable_from_param(function, param)
                    );

                    param = param->prev;
                }
            }
            // any non-handled params
            if(param != NULL) {
                parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_TYPE;
                return false;
            }
    );


    CODE_GENERATION(
            {
                GENERATE_CODE(I_PUSH_FRAME);
                String* function_label = symbol_function_generate_function_label(function);
                GENERATE_CODE(
                        I_CALL,
                        code_instruction_operand_init_label(string_content(function_label))
                );
                string_free(&function_label);
                GENERATE_CODE(I_POP_FRAME);
            }
    );

    ExprToken* e = create_expression((*expression_idx)++);
    if(function != NULL) {
        e->data_type = function->return_data_type;
    }
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
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_ADD);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_ADD);

    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    CodeConstructor* constructor = parser->code_constructor;
    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    } else {
        if(EXPR_LOWER_OPERAND->data_type == DATA_TYPE_STRING && EXPR_HIGHER_OPERAND->data_type == DATA_TYPE_STRING) {
            GENERATE_CODE(I_POP_STACK, code_instruction_operand_init_variable(parser->parser_semantic->temp_variable1));
            GENERATE_CODE(I_POP_STACK, code_instruction_operand_init_variable(parser->parser_semantic->temp_variable2));
            GENERATE_CODE(
                    I_CONCAT_STRING,
                    code_instruction_operand_init_variable(parser->parser_semantic->temp_variable1),
                    code_instruction_operand_init_variable(parser->parser_semantic->temp_variable2),
                    code_instruction_operand_init_variable(parser->parser_semantic->temp_variable1)
            );
            GENERATE_CODE(
                    I_PUSH_STACK,
                    code_instruction_operand_init_variable(parser->parser_semantic->temp_variable1)
            );
        } else {
            GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
            GENERATE_CODE(I_ADD_STACK);
        }
    }

    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_sub(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E - E
    */
    // backward
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_MINUS);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_SUB);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_SUB);

    CodeConstructor* constructor = parser->code_constructor;
    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    } else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
        GENERATE_CODE(I_SUB_STACK);
    }

    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_mul(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E * E
    */
    // backward
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_MULTIPLY);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_MULTIPLY);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_MULTIPLY);

    CodeConstructor* constructor = parser->code_constructor;

    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    } else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
        GENERATE_CODE(I_MUL_STACK);
    }

    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_div(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E / E
    */
    // backward
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_DIVIDE);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_DIVIDE);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_DIVIDE);

    CodeConstructor* constructor = parser->code_constructor;
    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    } else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
        GENERATE_CODE(I_DIV_STACK);
    }

    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_div_int(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E \ E
    */
    // backward
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_INTEGER_DIVIDE);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_INT_DIVIDE);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_INT_DIVIDE);

    CodeConstructor* constructor = parser->code_constructor;
    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    } else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
        GENERATE_CODE(I_DIV_STACK);
        GENERATE_CODE(I_FLOAT_TO_INT_STACK);
    }

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
    EXPR_CHECK_UNARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_UNARY_MINUS);

    ExprToken* e = create_expression((*expression_idx)++);

    CodeConstructor* constructor = parser->code_constructor;
    OperationSignature* signature = parser_semantic_get_operation_signature(
            parser->parser_semantic,
            OPERATION_UNARY_MINUS,
            EXPR_HIGHER_OPERAND->data_type,
            DATA_TYPE_NONE,
            DATA_TYPE_ANY
    );

    CodeInstructionOperand* operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_HIGHER_OPERAND,
            NULL,
            e,
            signature
    );

    if(operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, operand);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    } else {
        CodeInstructionOperand* inverse_operand = NULL;
        switch(EXPR_HIGHER_OPERAND->data_type) {
            case DATA_TYPE_INTEGER:
                inverse_operand = code_instruction_operand_init_integer(-1);
                break;
            case DATA_TYPE_DOUBLE:
                inverse_operand = code_instruction_operand_init_double(-1);
                break;
            default:
                // unknown operand to unary minus
                SEMANTIC_ANALYSIS(
                        {
                            parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_TYPE;
                            return false;
                        }
                );

        }
        CODE_GENERATION(
                {
                    GENERATE_CODE(I_PUSH_STACK, inverse_operand);
                    GENERATE_CODE(I_MUL_STACK);
                }
        );
        e->data_type = EXPR_HIGHER_OPERAND->data_type;
    }
    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_equal(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
     * RULE
     * E -> E = E
     */

    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_EQUAL);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_EQUAL);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_EQUAL);

    CodeConstructor* constructor = parser->code_constructor;
    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    }

    else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
        GENERATE_CODE(I_EQUAL_STACK);
    }

    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_not_equal(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
     * RULE
     * E -> E <> E
     */

    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_NOT_EQUAL);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_NOT_EQUAL);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_NOT_EQUAL);

    CodeConstructor* constructor = parser->code_constructor;
    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    }

    else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
        GENERATE_CODE(I_EQUAL_STACK);
        GENERATE_CODE(I_NOT_STACK);
    }

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
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_GREATER);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_GREATER);

    CodeConstructor* constructor = parser->code_constructor;
    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    }

    else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
        GENERATE_CODE(I_GREATER_THEN_STACK);
    }

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
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_GREATER_OR_EQUAL);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_GREATER_OR_EQUAL);

    CodeConstructor* constructor = parser->code_constructor;
    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    }

    else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();

        GENERATE_CODE(I_LESSER_THEN_STACK);
        GENERATE_CODE(I_NOT_STACK);
    }

    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_lesser(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
     * RULE
     * E -> E < E
     */

    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_LESSER);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_LESSER);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_LESSER);

    CodeConstructor* constructor = parser->code_constructor;
    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    }

    else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();

        GENERATE_CODE(I_LESSER_THEN_STACK);
    }

    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_lesser_or_equal(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
     * RULE
     * E -> E <= E
     */

    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_LESSER_OR_EQUAL);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_LESSER_OR_EQUAL);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_LESSER_OR_EQUAL);

    // generate conversion
    CodeConstructor* constructor = parser->code_constructor;
    CodeInstructionOperand* evaluated_operand = code_optimizer_expr_eval(
            parser->optimizer,
            EXPR_LOWER_OPERAND,
            EXPR_HIGHER_OPERAND,
            e,
            operation_signature
    );

    if(evaluated_operand != NULL) {
        code_generator_remove_instruction(constructor->generator, EXPR_LOWER_OPERAND->instruction);
        code_generator_remove_instruction(constructor->generator, EXPR_HIGHER_OPERAND->instruction);
    }

    if(evaluated_operand != NULL) {
        e->instruction = GENERATE_CODE(I_PUSH_STACK, evaluated_operand);
    }

    else {
        GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();

        GENERATE_CODE(I_GREATER_THEN_STACK);
        GENERATE_CODE(I_NOT_STACK);
    }

    EXPR_RULE_REPLACE(e);

    return true;
}

bool expression_rule_fn_length(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> length ( E )
    */
    CodeConstructor* constructor = parser->code_constructor;
    UNUSED(parser);
    UNUSED(constructor);

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_RIGHT_BRACKET);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_LEFT_BRACKET);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_FN_LENGTH);
    EXPR_RULE_CHECK_FINISH();

    // Length(s As String) As Integer
    const DataType param_data_type = get_n_expr(expr_token_buffer, 2)->data_type;
    EXPR_CHECK_UNARY_OPERATION_IMPLICIT_CONVERSION_FROM_DATA_TYPE(
            OPERATION_IMPLICIT_CONVERSION,
            param_data_type,
            DATA_TYPE_STRING
    );


    CODE_GENERATION(
            {
                code_constructor_fn_length(parser->code_constructor, parser->parser_semantic->temp_variable1,
                                           param_data_type);
            }
    );

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = DATA_TYPE_INTEGER;
    EXPR_RULE_REPLACE(e);
    return true;

}

bool expression_rule_fn_substr(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> substr ( E, E, E )
    */
    CodeConstructor* constructor = parser->code_constructor;
    UNUSED(parser);
    UNUSED(constructor);

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_RIGHT_BRACKET);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_COMMA);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_COMMA);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_LEFT_BRACKET);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_FN_SUBSTR);
    EXPR_RULE_CHECK_FINISH();

    DataType params_data_types[3];
    const unsigned int params_count = sizeof(params_data_types) / sizeof(*params_data_types);
    // note it's NOT backwards
    const DataType desired_params_data_types[3] = {
            DATA_TYPE_STRING, DATA_TYPE_INTEGER, DATA_TYPE_INTEGER
    };

    for(unsigned int i = 1; i <= params_count; i++) {
        params_data_types[i - 1] = get_n_expr(expr_token_buffer, (params_count - i + 1) * 2)
                ->data_type;
        EXPR_CHECK_UNARY_OPERATION_IMPLICIT_CONVERSION_FROM_DATA_TYPE(
                OPERATION_IMPLICIT_CONVERSION,
                params_data_types[i - 1],
                desired_params_data_types[i - 1]
        );
    }

    // NOTE: now we are processing rule regular way - from the left to the right

    // SubStr(s As String, i As Integer, n As Integer) As String

    code_constructor_fn_substr(
            parser->code_constructor,
            parser->parser_semantic->temp_variable1,
            parser->parser_semantic->temp_variable2,
            parser->parser_semantic->temp_variable3,
            parser->parser_semantic->temp_variable4,
            parser->parser_semantic->temp_variable5,
            params_data_types[0],
            params_data_types[1],
            params_data_types[2]
    );

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = DATA_TYPE_STRING;
    EXPR_RULE_REPLACE(e);
    return true;

}

bool expression_rule_fn_asc(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> asc ( E, E )
    */
    CodeConstructor* constructor = parser->code_constructor;
    UNUSED(parser);
    UNUSED(constructor);

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_RIGHT_BRACKET);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_COMMA);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_LEFT_BRACKET);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_FN_ASC);
    EXPR_RULE_CHECK_FINISH();


    // Asc(s As String, i As Integer) As Integer
    DataType params_data_types[2];
    const unsigned int params_count = sizeof(params_data_types) / sizeof(*params_data_types);
    // note it's NOT backwards
    const DataType desired_params_data_types[2] = {
            DATA_TYPE_STRING, DATA_TYPE_INTEGER
    };

    for(unsigned int i = 1; i <= params_count; i++) {
        params_data_types[i - 1] = get_n_expr(expr_token_buffer, (params_count - i + 1) * 2)
                ->data_type;
        EXPR_CHECK_UNARY_OPERATION_IMPLICIT_CONVERSION_FROM_DATA_TYPE(
                OPERATION_IMPLICIT_CONVERSION,
                params_data_types[i - 1],
                desired_params_data_types[i - 1]
        );
    }

    CODE_GENERATION(
            {
                code_constructor_fn_asc(
                        parser->code_constructor,
                        parser->parser_semantic->temp_variable1,
                        parser->parser_semantic->temp_variable2,
                        parser->parser_semantic->temp_variable3,
                        params_data_types[0],
                        params_data_types[1]
                );
            }
    );

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = DATA_TYPE_INTEGER;
    EXPR_RULE_REPLACE(e);
    return true;

}

bool expression_rule_fn_chr(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> chr ( E )
    */
    CodeConstructor* constructor = parser->code_constructor;
    UNUSED(parser);
    UNUSED(constructor);

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_RIGHT_BRACKET);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_LEFT_BRACKET);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_FN_CHR);
    EXPR_RULE_CHECK_FINISH();

    // NOTE: now we are processing rule regular way - from the left to the right

    // Chr(i As Integer) As String
    DataType param_data_type = get_n_expr(expr_token_buffer, 2)->data_type;
    EXPR_CHECK_UNARY_OPERATION_IMPLICIT_CONVERSION_FROM_DATA_TYPE(
            OPERATION_IMPLICIT_CONVERSION,
            param_data_type,
            DATA_TYPE_INTEGER
    );

    CODE_GENERATION(
            {
                code_constructor_fn_chr(parser->code_constructor, parser->parser_semantic->temp_variable1,
                                        param_data_type);
            }
    );

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = DATA_TYPE_STRING;
    EXPR_RULE_REPLACE(e);
    return true;

}

bool expression_rule_not(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> NOT E
    */

    // NOTE: we are processing rule backwards!
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_NOT);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_UNARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_NOT);

    // NOTE: now we are processing rule regular way - from the left to the right

    CODE_GENERATION(
            {
                CodeConstructor* constructor = parser->code_constructor;
                GENERATE_CODE(I_NOT_STACK);
            }
    );

    ExprToken* e = create_expression((*expression_idx)++);
    e->data_type = DATA_TYPE_BOOLEAN;
    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_and(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E AND E
    */
    // backward
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_AND);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_AND);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_AND);

    CodeConstructor* constructor = parser->code_constructor;
    GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
    GENERATE_CODE(I_AND_STACK);

    EXPR_RULE_REPLACE(e);
    return true;
}

bool expression_rule_or(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    /*
    * RULE
    * E -> E OR E
    */
    // backward
    EXPR_RULE_CHECK_START();
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_TYPE(EXPR_TOKEN_OR);
    EXPR_RULE_CHECK_TYPE(EXPR_EXPRESSION);
    EXPR_RULE_CHECK_FINISH();
    EXPR_CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(OPERATION_OR);

    CREATE_EXPR_RESULT_OF_BINARY_OPERATION(OPERATION_OR);

    CodeConstructor* constructor = parser->code_constructor;
    GENERATE_IMPLICIT_CONVERSIONS_FOR_BINARY_OPERATION_SIGNATURE();
    GENERATE_CODE(I_OR_STACK);

    EXPR_RULE_REPLACE(e);
    return true;
}
