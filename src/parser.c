#include "parser.h"
#include "parser_semantic.h"
#include "lexer.h"
#include "lexer_fsm.h"
#include "token.h"
#include "parser_expr.h"


Parser* parser_init(lexer_input_stream_f input_stream) {
    NULL_POINTER_CHECK(input_stream, NULL);
    Parser* parser = (Parser*) memory_alloc(sizeof(Parser));

    parser->lexer = lexer_init(input_stream);
    parser->error_report.error_code = ERROR_NONE;
    parser->parser_semantic = parser_semantic_init();
    parser->code_constructor = code_constructor_init();
    parser->run_type = PARSER_RUN_TYPE_ALL;
    parser->body_statement = false;
    return parser;
}

void parser_free(Parser** parser) {
    NULL_POINTER_CHECK(parser,);
    NULL_POINTER_CHECK(*parser,);
    lexer_free(&((*parser)->lexer));
    parser_semantic_free(&((*parser)->parser_semantic));
    code_constructor_free(&((*parser)->code_constructor));
    memory_free(*parser);
    *parser = NULL;
}

bool parser_parse(Parser* parser) {
    NULL_POINTER_CHECK(parser, false);

    if(!parser_parse_program(parser)) {

        // Toggle the corresponding error
        if(parser->parser_semantic->error_report.error_code != ERROR_NONE) {
            parser->error_report = parser->parser_semantic->error_report;
        } else if(parser->lexer->error_report.error_code != ERROR_NONE) {
            parser->error_report = parser->lexer->error_report;
        } else {
            parser->error_report.error_code = ERROR_SYNTAX;
        }

        // Set correct line
        parser->error_report.line = parser->lexer->lexer_fsm->line;

        return false;
    }
    ASSERT(parser->code_constructor->code_label_stack->head == NULL);
    return true;

}

bool parser_parse_program(Parser* parser) {
    /*
     * RULE
     * <prog> -> <body> <eols> EOF
     */
    CODE_GENERATION(
            {
                code_constructor_start_code(parser->code_constructor);
            }
    );
    // Call rule <body>. If <body> return false => return false
    RULES(
            CHECK_RULE(body);
            CHECK_RULE(eols);
            // Expect EOF token If return true, program is syntactically correct
            CHECK_TOKEN(TOKEN_EOF);
    );

    return true;
}

bool parser_parse_body(Parser* parser) {
    /*
     * RULE
     * <body> -> <definitions> <scope>
     */
    RULES(
            CHECK_RULE(definitions);

            SEMANTIC_ANALYSIS(
                    {
                        if(!parser_semantic_check_definitions(parser->parser_semantic))
                            return false;
                    }
            );


            parser->body_statement = true;
            CHECK_RULE(scope);
            UNUSED(token_type);
    );
    return true;
}

bool parser_parse_scope(Parser* parser) {
    /*
     * RULE
     * <scope> -> SCOPE EOL <eols> <statements> END SCOPE
     */
    RULES(
            CHECK_TOKEN(TOKEN_SCOPE);
            CODE_GENERATION(
                    {
                            code_constructor_main_scope_start(parser->code_constructor);
                    }
            );
            CHECK_TOKEN(TOKEN_EOL);
            CHECK_RULE(eols);
            CHECK_RULE(body_statements);
            CHECK_TOKEN(TOKEN_END);
            CHECK_TOKEN(TOKEN_SCOPE);
    );
    return true;
}

bool parser_parse_definitions(Parser* parser) {
    /*
     * RULES
     *
     * <definitions> -> <eols> <definition> <definitions>
     * <definitions> -> <eols> E
     */
    RULES(
            CHECK_RULE(eols);
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(token_type != TOKEN_DECLARE && token_type != TOKEN_FUNCTION, epsilon, NO_CODE);
            CHECK_RULE(definition);
            CHECK_RULE(definitions);
    );
    );

    return true;
}

bool parser_parse_definition(Parser* parser) {
    /*
     * RULES
     * <definition> -> <function_declaration>
     * <definition> -> <function_definition>
     */

    RULES(
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(token_type == TOKEN_DECLARE, function_declaration, NO_CODE);
            CHECK_RULE(token_type == TOKEN_FUNCTION, function_definition, NO_CODE);
            CHECK_RULE(epsilon);
    );
    );

    return true;
}

bool parser_parse_function_definition(Parser* parser) {
    /*
     * RULE
     * <function_definition> -> <function_header> EOL <eols> <statements> END FUNCTION
     */
    RULES(
            SEMANTIC_ANALYSIS(
                    {

                            parser_semantic_set_action(
                                    parser->parser_semantic,
                                    SEMANTIC_ACTION__FUNCTION_DEFINITION
                            );
                    }
            );
            CHECK_RULE(function_header);
            CHECK_TOKEN(TOKEN_EOL);
            CHECK_RULE(eols);
            CHECK_RULE(function_statements);
            CHECK_TOKEN(TOKEN_END);
            CHECK_TOKEN(TOKEN_FUNCTION);
    );
    return true;
}

bool parser_parse_function_statements(Parser* parser) {
    /*
     * RULES
     * <statements> -> E
     * <statements> -> <statement_single> EOL <eols> <statements>
     */
    RULES(
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);


            CHECK_RULE(
                    token_type != TOKEN_INPUT && token_type != TOKEN_RETURN && token_type != TOKEN_IF &&
                    token_type != TOKEN_DIM && token_type != TOKEN_PRINT && token_type != TOKEN_DO &&
                    token_type != TOKEN_IDENTIFIER,
                    epsilon,
                    NO_CODE
            );

            CHECK_RULE(function_statement_single);
            CHECK_TOKEN(TOKEN_EOL);
            CHECK_RULE(eols);
            CHECK_RULE(function_statements);
    );
    );
    return true;
}

bool parser_parse_body_statements(Parser* parser) {
    /*
     * RULES
     * <statements> -> E
     * <statements> -> <statement_single> EOL <eols> <statements>
     */

    RULES(
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);

            CHECK_RULE(
                    token_type != TOKEN_INPUT && token_type != TOKEN_DIM && token_type != TOKEN_PRINT &&
                    token_type != TOKEN_DO && token_type != TOKEN_IF && token_type != TOKEN_SCOPE &&
                    token_type != TOKEN_PRINT && token_type != TOKEN_IDENTIFIER,
                    epsilon,
                    BEFORE({}),
                    AFTER(
                            {
                                    token_free(&token);
                                    return true;
                            }
                    )
            );
            CHECK_RULE(body_statement_single);
            CHECK_TOKEN(TOKEN_EOL);
            CHECK_RULE(eols);
            CHECK_RULE(body_statements);
    );
    );
    return true;
}

bool parser_parse_function_statement_single(Parser* parser) {
    /*
     * RULE
     * <statement_single> -> INPUT <id>
     * <statement_single> -> RETURN <expr>
     * <statement_single> -> <variable_declaration>
     * <statement_single> -> <statement_print>
     */
    RULES(
            CONDITIONAL_RULES(
            CHECK_RULE(token_type == TOKEN_IDENTIFIER, identif_assignment, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_INPUT, input, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_RETURN, return_, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_PRINT, print, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_IF, condition, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_DO, while_, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_DIM, variable_declaration, REWIND_AND_SUCCESS);
    );
    );
    return false;
}

bool parser_parse_body_statement_single(Parser* parser) {
    /*
     * RULE
     * <statement_single> -> INPUT <id>
     */

    RULES(
            CONDITIONAL_RULES(
            CHECK_RULE(token_type == TOKEN_INPUT, input, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_IDENTIFIER, identif_assignment, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_DO, while_, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_PRINT, print, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_SCOPE, scope, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_IF, condition, REWIND_AND_SUCCESS);
            CHECK_RULE(token_type == TOKEN_DIM, variable_declaration, REWIND_AND_SUCCESS);
    );
    );

    return false;
}


bool parser_parse_function_declaration(Parser* parser) {
    /*
     * RULE
     * <function_declaration> -> DECLARE <function_header> EOL <eols>
     */
    RULES(
            CHECK_TOKEN(TOKEN_DECLARE);
            SEMANTIC_ANALYSIS(
                    {
                            parser_semantic_set_action(
                                    parser->parser_semantic,
                                    SEMANTIC_ACTION__FUNCTION_DECLARATION
                            );
                    }
            );
            CHECK_RULE(function_header);
            CHECK_TOKEN(TOKEN_EOL);
            CHECK_RULE(eols);
    );
    return true;
}

bool parser_parse_function_header(Parser* parser) {
    /*
     * RULE
     * FUNCTION IDENTIFIER (<function_params>) AS <type>
     */
    RULES(
            CHECK_TOKEN(TOKEN_FUNCTION);
            CHECK_TOKEN(TOKEN_IDENTIFIER);
            SEMANTIC_ANALYSIS(
                    {

                            if(!parser_semantic_set_function_name(parser->parser_semantic, token.data)) {
                        token_free(&token);
                        return false;
                    }
                    }
            );
            CHECK_TOKEN(TOKEN_LEFT_BRACKET);
            CHECK_RULE(function_params);
            SEMANTIC_ANALYSIS(
                    {
                            if(!parser_semantic_check_count_of_function_arguments(parser->parser_semantic)) {
                        token_free(&token);
                        return false;
                    }
                    }
            );

            CHECK_TOKEN(TOKEN_RIGHT_BRACKET);
            CHECK_TOKEN(TOKEN_AS);
            CHECK_TOKEN(TOKEN_DATA_TYPE_CLASS);
            SEMANTIC_ANALYSIS(
                    {
                            // TODO: resolve token_type -> data_type conversion for boolean
                            if(!parser_semantic_set_function_return_data_type(parser->parser_semantic, (DataType) token_type)) {
                        token_free(&token);
                        return false;
                    }
                    }
            );
    );
    return true;
}

bool parser_parse_function_params(Parser* parser) {
    /*
     * RULE
     * E
     * <function_param> <function_n_param>
     */
    RULES(
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(
                    token_type == TOKEN_RIGHT_BRACKET,
                    epsilon,
                    BEFORE(),
                    AFTER({
                                  return true;
                          }
                    )
            );
            CHECK_RULE(function_param);
            CHECK_RULE(function_n_param);
    );
    );
    return true;
}

bool parser_parse_function_n_param(Parser* parser) {
    /*
     * RULE
     * E
     * <function_param> <function_n_param>
     */
    RULES(
            CONDITIONAL_RULES(
                    CHECK_RULE(
                            token_type == TOKEN_RIGHT_BRACKET,
                            epsilon,
                            BEFORE(),
                            AFTER({
                                          lexer_rewind_token(
                                                  parser->lexer,
                                                  token
                                          );
                                          return true;
                                  }
                            )
                    );
            CHECK_RULE(function_param);
            CHECK_RULE(function_n_param);
    );
    );
    return true;
}

bool parser_parse_function_param(Parser* parser) {
    /*
     * RULE
     * <function_param> -> IDENTIFIER AS TYPE
     */
    char* name = NULL;
    RULES(
            CHECK_TOKEN(TOKEN_IDENTIFIER);
            name = c_string_copy(token.data);
            memory_free_lazy(name);
            CHECK_TOKEN(TOKEN_AS);
            CHECK_TOKEN(TOKEN_DATA_TYPE_CLASS);
            SEMANTIC_ANALYSIS(
                    {
                            if(!parser_semantic_add_function_parameter(parser->parser_semantic, name, (DataType) token_type))
                            return false;
                    }
            );
    );

    return true;
}

bool parser_parse_eols(Parser* parser) {
    /*
     * RULES
     * <eols> -> E
     * <eols> -> EOL <eols>
     */
    RULES(
            CONDITIONAL_RULES(
                    CHECK_RULE(token_type == TOKEN_EOL, eols, NO_CODE);
            CHECK_RULE(
                    epsilon,
                    BEFORE(),
                    AFTER(
                            {
                                    lexer_rewind_token(parser->lexer, token);
                            }
                    )
            );
    );
    );
    return true;
}

bool parser_parse_epsilon(Parser* parser) {
    UNUSED(parser);
    return true;
}

bool parser_parse_variable_declaration(Parser* parser) {
    /**
     * RULES
     * <variable_declaration> -> DIM IDENTIFIER AS <type>
     */
    RULES(
            char* name = NULL;
            SymbolVariable* variable = NULL;
            CHECK_TOKEN(TOKEN_DIM);
            CHECK_TOKEN(
                    TOKEN_IDENTIFIER,
                    BEFORE({
                                   name = c_string_copy(token.data);
                                   memory_free_lazy(name);
                           })
            );
            CHECK_TOKEN(TOKEN_AS);
            CHECK_TOKEN(
                    TOKEN_DATA_TYPE_CLASS,
                    BEFORE(),
                    {
                            SEMANTIC_ANALYSIS(
                                    {
                                            // TODO: resolve token_type -> data_type conversion for boolean
                                            variable = parser_semantic_add_symbol_variable(
                                                    parser->parser_semantic,
                                                    name,
                                                    (DataType) token_type
                                            );

                                            if(variable == NULL) {
                                        return false;
                                    }
                                    }
                            );
                            CODE_GENERATION(
                            {
                                code_constructor_variable_declaration(
                                        parser->code_constructor,
                                        parser->parser_semantic->register_->index_of_found_variable,
                                        variable
                                );
                            }
                    );}
            );
            CALL_RULE(declaration_assignment);
    );
    return true;
}

bool parser_parse_return_(Parser* parser) {
    /*
     * RULE
     * <statement> -> return <expr>
     */
    RULES(
            CHECK_TOKEN(TOKEN_RETURN);
            CALL_RULE(expression);
    );
    return true;
}

bool parser_parse_print(Parser* parser) {
    /*
     * RULE
     * <statement_print> -> PRINT <print_expression> <print_expressions>
     */
    RULES(
            CHECK_TOKEN(TOKEN_PRINT);
            CALL_RULE(print_expression);
            CALL_RULE(print_expressions);
    );
    return true;
}

bool parser_parse_print_expressions(Parser* parser) {

    RULES(
            CONDITIONAL_RULES(
                    CHECK_RULE(
                            token_type == TOKEN_EOL,
                            epsilon,
                            BEFORE(
                                    {
                                            lexer_rewind_token(parser->lexer,
                                                               token);
                                    }),
                            AFTER(
                                    {
                                            token_free(&token);
                                            return true;
                                    }
                            )
                    );
            CHECK_RULE(
                    token_type != TOKEN_EOL,
                    print_expression,
                    BEFORE(
                            {
                                    lexer_rewind_token(parser->lexer, token);
                            }
                    ), AFTER(
                            {
                                    token_free(&token);
                            }
                    )
            );
            CALL_RULE(print_expressions);
    );
    );
    return true;
}

bool parser_parse_print_expression(Parser* parser) {
    /*
     * RULE
     * <print_expression> -> <expression> SEMICOLON
     */
    RULES(
            CALL_RULE(expression);
            CODE_GENERATION(
                    {
                            code_constructor_print_expression(parser->code_constructor);
                    }
            );
            CHECK_TOKEN(TOKEN_SEMICOLON);
    );
    return true;
}

bool parser_parse_while_(Parser* parser) {
    /*
     * RULE
     * <do_while> -> DO WHILE <expression> EOL <eols> <statements> LOOP
     */
    RULES(
            CHECK_TOKEN(TOKEN_DO);
            CHECK_TOKEN(TOKEN_WHILE);
            CODE_GENERATION(
                    {
                            code_constructor_while_before_condition(parser->code_constructor);
                    }
            );
            CALL_RULE(expression);
            CODE_GENERATION(
                    {
                            code_constructor_while_after_condition(parser->code_constructor);
                    }
            );
            CHECK_TOKEN(TOKEN_EOL);
            CALL_RULE(eols);

            CALL_RULE_STATEMENTS();
            CHECK_TOKEN(TOKEN_LOOP);
            CODE_GENERATION(
                    {
                            code_constructor_while_end(parser->code_constructor);
                    }
            );
    );
    return true;
}


bool parser_parse_input(Parser* parser) {
    /*
     * RULE
     * <statement> -> input identifier
     */
    SymbolVariable* symbol_variable = NULL;

    RULES(
            CHECK_TOKEN(TOKEN_INPUT);
            CHECK_TOKEN(
                    TOKEN_IDENTIFIER,
                    BEFORE({}),
                    AFTER(
                            SEMANTIC_ANALYSIS(
                                    {
                                            symbol_variable = symbol_register_find_variable_recursive(
                                                    parser->parser_semantic->register_,
                                                    token.data
                                            );
                                            if(NULL == symbol_variable) {
                                        token_free(&token);
                                        return false;
                                    }
                                    }
                            );
            CODE_GENERATION(
                    {
                            code_constructor_input(
                                    parser->code_constructor,
                                    parser->parser_semantic->register_->index_of_found_variable,
                                    symbol_variable
                            );
                    }
            );
    ));
    );
    return true;
}

bool parser_parse_condition(Parser* parser) {
    RULES(
            CHECK_TOKEN(TOKEN_IF);
            CALL_RULE(expression);
            CODE_GENERATION(
                    {
                            code_constructor_if_after_expression(parser->code_constructor);
                    }
            );
            CHECK_TOKEN(TOKEN_THEN);
            CHECK_TOKEN(TOKEN_EOL);
            CALL_RULE(eols)
            CALL_RULE_STATEMENTS();
            CODE_GENERATION(
                    {
                            code_constructor_if_end_if_block(parser->code_constructor);
                    }
            );
            CALL_RULE(condition_elseif);
            CALL_RULE(condition_else);
            CHECK_TOKEN(TOKEN_END);
            CHECK_TOKEN(TOKEN_IF);
            CODE_GENERATION(
                    {
                            code_constructor_if_after_end_if(parser->code_constructor);
                    }
            );
    );
    return true;
}

bool parser_parse_condition_elseif(Parser* parser) {
    RULES(
            CONDITIONAL_RULES(
                    CHECK_RULE(
                            token_type != TOKEN_ELSEIF,
                            epsilon,
                            BEFORE(
                                    {
                                            lexer_rewind_token(parser->lexer, token);
                                    }
                            ),
                            AFTER(
                                    {
                                            token_free(&token);
                                            return true;
                                    }
                            )
                    );
            CODE_GENERATION(
                    {
                            code_constructor_if_else_if_before_expression(parser->code_constructor);
                    }
            );
            CALL_RULE(expression);
            CODE_GENERATION(
                    {
                            code_constructor_if_else_if_after_expression(parser->code_constructor);
                    }
            );
            CHECK_TOKEN(TOKEN_THEN);
            CHECK_TOKEN(TOKEN_EOL);
            CALL_RULE_STATEMENTS();
            CALL_RULE(condition_elseif);
    );
    );

    return true;
}


bool parser_parse_condition_else(Parser* parser) {
    RULES(
            CONDITIONAL_RULES(
                    CHECK_RULE(
                            token_type != TOKEN_ELSE,
                            epsilon,
                            BEFORE(
                                    {
                                            lexer_rewind_token(parser->lexer, token);
                                    }
                            ),
                            AFTER(
                                    {
                                            token_free(&token);
                                            return true;
                                    }
                            )
                    );
            CHECK_TOKEN(TOKEN_EOL);
            CALL_RULE(eols)
            CODE_GENERATION(
                    {
                            code_constructor_if_else_block(parser->code_constructor);
                    }
            );
            CALL_RULE_STATEMENTS();
    );
    );
    return true;
}

bool parser_parse_declaration_assignment(Parser* parser) {
    /*
     * RULE
     * <condition_assignment> -> E
     * <condition_assignment> -> <assignment>
     */

    RULES(

            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);
                    CHECK_RULE(token_type == TOKEN_EQUAL, assignment, NO_CODE);
        );
    );

    return true;
}

bool parser_parse_identif_assignment(Parser* parser) {
    /*
     * RULE
     * <identif_assignment> -> IDENTIF <assignment>
     */

    RULES(

            CHECK_TOKEN(TOKEN_IDENTIFIER);
            CHECK_RULE(assignment);
    );

    return true;
}



bool parser_parse_assignment(Parser* parser) {
    /*
     * RULE
     * <assignment> -> = <expression>
     */
    RULES(
            CHECK_TOKEN(TOKEN_EQUAL);
            CALL_RULE(expression);
    );
    return true;
}
