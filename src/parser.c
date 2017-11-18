#include "parser.h"
#include "parser_semantic.h"
#include "lexer.h"
#include "lexer_fsm.h"
#include "token.h"
#include "parser_expr.h"
#include "parser_expr_rules.h"


Parser* parser_init(lexer_input_stream_f input_stream) {
    NULL_POINTER_CHECK(input_stream, NULL);
    Parser* parser = (Parser*) memory_alloc(sizeof(Parser));

    parser->lexer = lexer_init(input_stream);
    parser->error_report.error_code = ERROR_NONE;
    parser->parser_semantic = parser_semantic_init();
    parser->code_constructor = code_constructor_init();
    parser->run_type = PARSER_RUN_TYPE_ALL;
    parser->body_statement = false;
    parser->error_report.error_code = ERROR_NONE;
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
    ASSERT(parser->code_constructor->loops_initial_instruction == NULL);
    ASSERT(parser->code_constructor->conditions_label_stack->head == NULL);
    ASSERT(parser->code_constructor->loops_label_stack->head == NULL);
    ASSERT(parser->code_constructor->control_statement_depth == 0);
    ASSERT(parser->code_constructor->loops_depth == 0);
    ASSERT(parser->code_constructor->scope_depth == 0);
    return true;
}

bool parser_parse_program(Parser* parser) {
    /*
     * RULE
     * <prog> -> <body> <eols> EOF
     */
    CODE_GENERATION(
            {
                parser_semantic_setup_temp_variables(parser->parser_semantic);

                code_constructor_variable_declaration(
                        parser->code_constructor,
                        parser->parser_semantic->temp_variable1
                );
                code_constructor_variable_declaration(
                        parser->code_constructor,
                        parser->parser_semantic->temp_variable2
                );
                code_constructor_variable_declaration(
                        parser->code_constructor,
                        parser->parser_semantic->temp_variable3
                );
                code_constructor_variable_declaration(
                        parser->code_constructor,
                        parser->parser_semantic->temp_variable4
                );
                code_constructor_variable_declaration(
                        parser->code_constructor,
                        parser->parser_semantic->temp_variable5
                );
            }
    );
    // Call rule <body>. If <body> return false => return false
    RULES(
            CHECK_RULE(body);
            CHECK_RULE(eols);
            // Expect EOF token If return true, program is syntactically correct
            CHECK_TOKEN(TOKEN_EOF);
    );

    CODE_GENERATION(
            {
                code_constructor_end_code(parser->code_constructor);
            }
    );

    return true;
}

bool parser_parse_body(Parser* parser) {
    /*
     * RULE
     * <body> -> <definitions> <scope> <shared_variables_declarations>
     */
    RULES(
            CHECK_RULE(definitions);

            SEMANTIC_ANALYSIS(
                    {
                            if(!parser_semantic_check_function_definitions(parser->parser_semantic))
                            return false;
                    }
            );


            parser->body_statement = true;
            CHECK_RULE(scope);
            CHECK_RULE(shared_variables_declarations);
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
                            code_constructor_scope_start(parser->code_constructor);
                            symbol_register_push_variables_table(parser->parser_semantic->register_);
                    }
            );
            CHECK_TOKEN(TOKEN_EOL);
            CHECK_RULE(eols);
            CHECK_RULE(body_statements);
            CHECK_TOKEN(TOKEN_END);
            CHECK_TOKEN(TOKEN_SCOPE);
            CODE_GENERATION(
                    {
                            code_constructor_scope_end(parser->code_constructor);
                            symbol_register_pop_variables_table(parser->parser_semantic->register_);
                    }
            );
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
            CHECK_RULE(token_type != TOKEN_DECLARE && token_type != TOKEN_FUNCTION && token_type != TOKEN_DIM, epsilon,
                       NO_CODE);
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
     * <definition> -> <shared_variable_declaration>
     */

    RULES(
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(token_type == TOKEN_DECLARE, function_declaration, NO_CODE);
            CHECK_RULE(token_type == TOKEN_FUNCTION, function_definition, NO_CODE);
            CHECK_RULE(token_type == TOKEN_DIM, shared_variable_declaration, NO_CODE);
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

    CodeInstruction* before_instruction = NULL;
    CodeConstructor* constructor = parser->code_constructor;
    UNUSED(constructor);

    CODE_GENERATION({
                        before_instruction = parser->code_constructor->generator->buffer_last;
                    });
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
            CODE_GENERATION(
                    {
                            code_constructor_function_header(
                                    parser->code_constructor,
                                    parser->parser_semantic->actual_function
                            );
                    }
            );
            SEMANTIC_ANALYSIS(
                    {
                            parser_semantic_function_start(
                                    parser->parser_semantic,
                                    parser->parser_semantic->actual_function
                            );
                    }
            );
            CHECK_TOKEN(TOKEN_EOL);
            CHECK_RULE(eols);
            CHECK_RULE(function_statements);
            CHECK_TOKEN(TOKEN_END);
            CHECK_TOKEN(TOKEN_FUNCTION);
            SEMANTIC_ANALYSIS(
                    {
                            parser_semantic_function_end(parser->parser_semantic);
                    }
            );
            CODE_GENERATION(
                    {
                            code_constructor_function_end(
                                    parser->code_constructor,
                                    parser->parser_semantic->actual_function
                            );

                    }
            );
    );

            CODE_GENERATION(
                {
                    if(before_instruction == NULL)
                        parser->code_constructor->generator->buffer_first
                            ->meta_data.type = CODE_INSTRUCTION_META_TYPE_FUNCTION_START;
                    else
                        before_instruction->next
                            ->meta_data.type = CODE_INSTRUCTION_META_TYPE_FUNCTION_START;

                    parser->code_constructor->generator->buffer_last
                            ->meta_data.type |= CODE_INSTRUCTION_META_TYPE_FUNCTION_END;
                }
            );

    parser->parser_semantic->actual_function = NULL;
    return true;
}

bool parser_parse_function_statements(Parser* parser) {
    /*
     * RULES
     * <function_statements> -> E
     * <function_statements> -> <function_statement_single> EOL <eols> <function_statements>
     */
    RULES(
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);


            CHECK_RULE(
                    token_type != TOKEN_INPUT && token_type != TOKEN_RETURN && token_type != TOKEN_IF &&
                    token_type != TOKEN_DIM && token_type != TOKEN_PRINT && token_type != TOKEN_DO &&
                    token_type != TOKEN_IDENTIFIER && token_type != TOKEN_STATIC,
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
     * <body_statements> -> E
     * <body_statements> -> <body_statement_single> EOL <eols> <body_statements>
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
     * <function_statement_single> -> <identifier_assignment>
     * <function_statement_single> -> <input>
     * <function_statement_single> -> <return>
     * <function_statement_single> -> <print>
     * <function_statement_single> -> <condition>
     * <function_statement_single> -> <while_>
     * <function_statement_single> -> <variable_declaration>
     * <function_statement_single> -> <static_variable_declaration>
     */
    RULES(
            CONDITIONAL_RULES(
                    CHECK_RULE(token_type == TOKEN_IDENTIFIER, identifier_assignment, REWIND_AND_SUCCESS);
                    CHECK_RULE(token_type == TOKEN_INPUT, input, REWIND_AND_SUCCESS);
                    CHECK_RULE(token_type == TOKEN_RETURN, return_, REWIND_AND_SUCCESS);
                    CHECK_RULE(token_type == TOKEN_PRINT, print, REWIND_AND_SUCCESS);
                    CHECK_RULE(token_type == TOKEN_IF, condition, REWIND_AND_SUCCESS);
                    CHECK_RULE(token_type == TOKEN_DO, while_, REWIND_AND_SUCCESS);
                    CHECK_RULE(token_type == TOKEN_DIM, variable_declaration, REWIND_AND_SUCCESS);
                    CHECK_RULE(token_type == TOKEN_STATIC, static_variable_declaration, REWIND_AND_SUCCESS);
            );
    );
    return false;
}

bool parser_parse_body_statement_single(Parser* parser) {
    /*
     * RULE
     * <body_statement_single> -> <input>
     * <body_statement_single> -> <identifier_assignment>
     * <body_statement_single> -> <while_>
     * <body_statement_single> -> <print>
     * <body_statement_single> -> <scope>
     * <body_statement_single> -> <condition>
     * <body_statement_single> -> <variable_declaration>
     */

    RULES(
            CONDITIONAL_RULES(
                    CHECK_RULE(token_type == TOKEN_INPUT, input, REWIND_AND_SUCCESS);
                    CHECK_RULE(token_type == TOKEN_IDENTIFIER, identifier_assignment, REWIND_AND_SUCCESS);
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
     * <function_header> -> FUNCTION IDENTIFIER (<function_params>) AS <type>
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
     * <function_params> -> E
     * <function_params> -> <function_param> <function_n_param>
     */
    RULES(
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);
            CHECK_RULE(
                    token_type == TOKEN_RIGHT_BRACKET,
                    epsilon,
                    BEFORE({}),
                    AFTER(
                            {
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
     * <function_n_param> -> E
     * <function_n_param> -> <function_param> <function_n_param>
     */
    RULES(
            CONDITIONAL_RULES(
                    CHECK_RULE(
                            token_type == TOKEN_RIGHT_BRACKET,
                            epsilon,
                            BEFORE({}),
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
                    BEFORE({}),
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
    /*
     * RULE
     * <variable_declaration> -> DIM IDENTIFIER AS <type> <declaration_assignment>
     */

    char* name = NULL;
    RULES(
            CHECK_TOKEN(TOKEN_DIM);
            CHECK_TOKEN(
                    TOKEN_IDENTIFIER,
                    BEFORE(
                            {
                                    name = c_string_copy(token.data);
                                    memory_free_lazy(name);
                            }
                    )
            );
            CHECK_TOKEN(TOKEN_AS);
            CHECK_TOKEN(TOKEN_DATA_TYPE_CLASS);
            SEMANTIC_ANALYSIS(
                    {
                            // TODO: resolve token_type -> data_type conversion for boolean
                            parser->parser_semantic->actual_variable = parser_semantic_add_variable(
                                    parser->parser_semantic,
                                    name,
                                    (DataType) token_type
                            );

                            if(parser->parser_semantic->actual_variable == NULL) {
                        token_free(&token);
                        return false;
                    }
                    }
            );
            CODE_GENERATION(
                    {

                            parser->parser_semantic->actual_variable->frame = VARIABLE_FRAME_LOCAL;
                            code_constructor_variable_declaration(
                            parser->code_constructor,
                            parser->parser_semantic->actual_variable
                    );
                    }
            );
            CALL_RULE(declaration_assignment);
    );
    return true;
}

bool parser_parse_shared_variables_declarations(Parser* parser) {
    /*
     * RULE
     * <shared_variables_declarations> -> E
     * <shared_variables_declarations> -> <shared_variable_declaration>
     */

    RULES(
            CHECK_RULE(eols);
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);
                    CHECK_RULE(token_type != TOKEN_DIM, epsilon, NO_CODE);
                    CHECK_RULE(shared_variable_declaration);
                    CHECK_RULE(shared_variables_declarations);
            );
    );

    return true;
}

bool parser_parse_shared_variable_declaration(Parser* parser) {
    /*
     * RULE
     * <shared_variable_declaration> -> DIM SHARED IDENTIFIER AS <type>
     */

    char* name = NULL;
    RULES(
            CHECK_TOKEN(TOKEN_DIM);
            CHECK_TOKEN(TOKEN_SHARED);
            CHECK_TOKEN(
                    TOKEN_IDENTIFIER,
                    BEFORE(
                            {
                                    name = c_string_copy(token.data);
                                    memory_free_lazy(name);
                            }
                    )
            );
            CHECK_TOKEN(TOKEN_AS);
            CHECK_TOKEN(TOKEN_DATA_TYPE_CLASS);
            SEMANTIC_ANALYSIS(
                    {
                            parser->parser_semantic->actual_variable = parser_semantic_add_variable(
                                    parser->parser_semantic,
                                    name,
                                    (DataType) token_type
                            );

                            if(parser->parser_semantic->actual_variable == NULL) {
                        token_free(&token);
                        return false;
                    }
                    }
            );
            CODE_GENERATION(
                    {

                            parser->parser_semantic->actual_variable->frame = VARIABLE_FRAME_GLOBAL;
                            code_constructor_shared_variable_declaration(
                            parser->code_constructor,
                            parser->parser_semantic->actual_variable
                    );
                    }
            );
            CALL_RULE(declaration_assignment);
    );
    return true;
}

bool parser_parse_static_variable_declaration(Parser* parser) {
    /*
     * RULE
     * <static_variable_declaration> -> STATIC IDENTIFIER AS <type> <declaration_assignment>
     */

    char* name = NULL;
    RULES(
            CHECK_TOKEN(TOKEN_STATIC);
            CHECK_TOKEN(
                    TOKEN_IDENTIFIER,
                    BEFORE(
                            {
                                    name = c_string_copy(token.data);
                                    memory_free_lazy(name);
                            }
                    )
            );
            CHECK_TOKEN(TOKEN_AS);
            CHECK_TOKEN(TOKEN_DATA_TYPE_CLASS);
            SEMANTIC_ANALYSIS(
                    {
                            parser->parser_semantic->actual_variable = parser_semantic_add_variable(
                                    parser->parser_semantic,
                                    name,
                                    (DataType) token_type
                            );

                            if(parser->parser_semantic->actual_variable == NULL) {
                        token_free(&token);
                        return false;
                    }
                    }
            );
            CODE_GENERATION(
                    {

                            parser->parser_semantic->actual_variable->frame = VARIABLE_FRAME_LOCAL;
                            code_constructor_static_variable_declaration(
                            parser->code_constructor,
                            parser->parser_semantic->actual_variable,
                            parser->parser_semantic->actual_function
                    );
                    }
            );
            CALL_RULE(declaration_assignment);
            CODE_GENERATION(
                    {
                            code_constructor_static_variable_declaration_end(
                                    parser->code_constructor,
                                    parser->parser_semantic->actual_variable,
                                    parser->parser_semantic->actual_function
                            );
                    }
            );
    );
    return true;
}

bool parser_parse_return_(Parser* parser) {
    /*
     * RULE
     * <statement> -> RETURN <expr>
     */
    DataType expression_data_type;
    CodeConstructor* constructor;
    RULES(
            CHECK_TOKEN(TOKEN_RETURN);
            CALL_EXPRESSION_RULE(expression_data_type);
            CODE_GENERATION(
                    {
                            constructor = parser->code_constructor;
                            GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(
                            expression_data_type,
                            parser->parser_semantic->actual_function->return_data_type
                    );
                    }
            );
            CODE_GENERATION(
                    {
                            code_constructor_return(parser->code_constructor);
                    }
            );
    );
    return true;
}

bool parser_parse_print(Parser* parser) {
    /*
     * RULE
     * <print> -> PRINT <print_expression> <print_expressions>
     */
    RULES(
            CHECK_TOKEN(TOKEN_PRINT);
            CALL_RULE(print_expression);
            CALL_RULE(print_expressions);
    );
    return true;
}

bool parser_parse_print_expressions(Parser* parser) {
    /*
     * RULE
     * <print_expressions> -> E
     * <print_expressions> -> <print_expression> <print_expressions>
     */
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
    DataType expression_data_type;

    RULES(
            CALL_EXPRESSION_RULE(expression_data_type);
            CODE_GENERATION(
                    {
                            code_constructor_print_expression(
                                    parser->code_constructor,
                                    parser->parser_semantic->temp_variable1
                            );
                    }
            );
            CHECK_TOKEN(TOKEN_SEMICOLON);
    );
    return true;
}

bool parser_parse_while_(Parser* parser) {
    /*
     * RULE
     * <while_> -> DO WHILE <expression> EOL <eols> <statements> LOOP
     */
    DataType expression_data_type;

    RULES(
            CHECK_TOKEN(TOKEN_DO);
            CHECK_TOKEN(TOKEN_WHILE);
            SEMANTIC_ANALYSIS(
                    {
                            symbol_register_push_variables_table(parser->parser_semantic->register_);
                    }
            );
            CODE_GENERATION(
                    {
                            code_constructor_while_before_condition(parser->code_constructor);
                    }
            );
            CALL_EXPRESSION_RULE(expression_data_type);
            SEMANTIC_ANALYSIS(
                    {
                            CHECK_IMPLICIT_CONVERSION(DATA_TYPE_BOOLEAN, expression_data_type);
                    }
            );
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
            SEMANTIC_ANALYSIS(
                    {
                            symbol_register_pop_variables_table(parser->parser_semantic->register_);
                    }
            );
    );
    return true;
}


bool parser_parse_input(Parser* parser) {
    /*
     * RULE
     * <input> -> INPUT IDENTIFIER
     */
    SymbolVariable* symbol_variable = NULL;

    RULES(
            CHECK_TOKEN(TOKEN_INPUT);
            CHECK_TOKEN(TOKEN_IDENTIFIER);
            SEMANTIC_ANALYSIS(
                    {
                            symbol_variable = symbol_register_find_variable_recursive(
                                    parser->parser_semantic->register_,
                                    token.data
                            );
                            if(NULL == symbol_variable) {
                        parser->parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
                        token_free(&token);
                        return false;
                    }
                    }
            );
    );

    CODE_GENERATION(
            {
                    code_constructor_input(parser->code_constructor, symbol_variable);
            }
    );
    return true;
}

bool parser_parse_condition(Parser* parser) {
    /*
     * RULE
     * <condition> -> IF <expr> THEN EOL <eols> <statements> <condition_elseif> <condition_else> END IF
     */
    DataType expression_data_type;

    RULES(
            CHECK_TOKEN(TOKEN_IF);
            CALL_EXPRESSION_RULE(expression_data_type);
            SEMANTIC_ANALYSIS(
                    {
                            CHECK_IMPLICIT_CONVERSION(DATA_TYPE_BOOLEAN, expression_data_type);
                    }
            );
            CODE_GENERATION(
                    {
                            code_constructor_if_after_expression(parser->code_constructor);
                    }
            );
            CHECK_TOKEN(TOKEN_THEN);
            CHECK_TOKEN(TOKEN_EOL);
            SEMANTIC_ANALYSIS(
                    {
                            symbol_register_push_variables_table(parser->parser_semantic->register_);
                    }
            );
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
            SEMANTIC_ANALYSIS(
                    {
                            symbol_register_pop_variables_table(parser->parser_semantic->register_);
                    }
            );
            CODE_GENERATION(
                    {
                            code_constructor_if_after_end_if(parser->code_constructor);
                    }
            );
    );
    return true;
}

bool parser_parse_condition_elseif(Parser* parser) {
    /*
     * RULE
     * <condition_elseif> -> E
     * <condition_elseif> -> ELSEIF <expr> THEN EOL <statements> <condition_elseif>
     */

    DataType expression_data_type;

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
            SEMANTIC_ANALYSIS(
                    {
                            symbol_register_pop_variables_table(parser->parser_semantic->register_);
                    }
            );
            CALL_EXPRESSION_RULE(expression_data_type);
            SEMANTIC_ANALYSIS(
                    {
                            CHECK_IMPLICIT_CONVERSION(DATA_TYPE_BOOLEAN, expression_data_type);
                    }
            );
            CODE_GENERATION(
                    {
                            code_constructor_if_else_if_after_expression(parser->code_constructor);
                    }
            );
            CHECK_TOKEN(TOKEN_THEN);
            CHECK_TOKEN(TOKEN_EOL);
            CALL_RULE(eols);
            CALL_RULE_STATEMENTS();
            SEMANTIC_ANALYSIS(
                    {
                            symbol_register_push_variables_table(parser->parser_semantic->register_);
                    }
            );
            CALL_RULE(condition_elseif);
    );
    );

    return true;
}


bool parser_parse_condition_else(Parser* parser) {
    /*
     * RULE
     * <condition_else> -> E
     * <condition_else> -> ELSE EOL <eols> <statements>
     */
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
            CALL_RULE(eols);
            CODE_GENERATION(
                    {
                            code_constructor_if_else_block(parser->code_constructor);
                    }
            );
            SEMANTIC_ANALYSIS(
                    {
                            symbol_register_push_variables_table(parser->parser_semantic->register_);
                    }
            );
            CALL_RULE_STATEMENTS();
            SEMANTIC_ANALYSIS(
                    {
                            symbol_register_pop_variables_table(parser->parser_semantic->register_);
                    }
            );
    );
    );
    return true;
}

bool parser_parse_declaration_assignment(Parser* parser) {
    /*
     * RULE
     * <declaration_assignment> -> E
     * <declaration_assignment> -> <assignment>
     */

    RULES(

            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);
                    CHECK_RULE(token_type == TOKEN_EQUAL, assignment, NO_CODE);
            );
    );
    return true;
}

bool parser_parse_identifier_assignment(Parser* parser) {
    /*
     * RULE
     * <identifier_assignment> -> IDENTIF <assignment>
     */

    RULES(
            CHECK_TOKEN(
                    TOKEN_IDENTIFIER,
                    BEFORE(
                            {
                                    parser->parser_semantic->actual_variable = symbol_register_find_variable_recursive(
                                            parser->parser_semantic->register_,
                                            token.data
                                    );
                            }
                    )
            );
            CHECK_RULE(assignment);
            parser->parser_semantic->actual_variable = NULL;
    );

    return true;
}


bool parser_parse_assignment(Parser* parser) {
    /*
     * RULE
     * <assignment> -> = <expression>
     * <assignment> -> <modify_assignment>
     */

    DataType expression_data_type;
    SymbolVariable* actual_variable = parser->parser_semantic->actual_variable;
    CodeConstructor* constructor = parser->code_constructor;
    UNUSED(constructor);

    RULES(
            CONDITIONAL_RULES(
                    lexer_rewind_token(parser->lexer, token);

            CHECK_RULE(
                    token_type == TOKEN_ASSIGN_SUB || token_type == TOKEN_ASSIGN_ADD ||
                    token_type == TOKEN_ASSIGN_DIVIDE || token_type == TOKEN_ASSIGN_MULTIPLY ||
                    token_type == TOKEN_ASSIGN_INT_DIVIDE,
                    modify_assignment,
                    BEFORE({}),
                    AFTER(
                            {
                                    token_free(&token);
                                    return true;
                            }
                    )
            );
    );

            CHECK_TOKEN(TOKEN_EQUAL);
            CodeInstruction* before_expression_instruction = NULL;
            UNUSED(before_expression_instruction);

            CODE_GENERATION(
                before_expression_instruction = code_generator_last_instruction(parser->code_constructor->generator);
            );
            CALL_EXPRESSION_RULE(expression_data_type);
            CODE_GENERATION(
                before_expression_instruction->next
                        ->meta_data.type |= CODE_INSTRUCTION_META_TYPE_EXPRESSION_START;
            );
    );
    SEMANTIC_ANALYSIS(
            {
                if(actual_variable == NULL)
                    return false;
                CHECK_IMPLICIT_CONVERSION(actual_variable->data_type, expression_data_type);
            }
    );

    CODE_GENERATION(
            {
                GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(
                        expression_data_type,
                        actual_variable->data_type
                );
                code_constructor_variable_expression_assignment(
                        parser->code_constructor,
                        actual_variable
                );
                code_generator_last_instruction(parser->code_constructor->generator)->meta_data.type |= CODE_INSTRUCTION_META_TYPE_EXPRESSION_END;
            }
    );

    return true;
}

#define SHORTEN_OPERATORS_COUNT 5
bool parser_parse_modify_assignment(Parser* parser) {
    /*
     * RULE
     * <assignment> -> <modify> <expression>
     * <modify> -> +=
     * <modify> -> -=
     * <modify> -> *=
     * <modify> -> /=
     * <modify> -> \=
     */

    const int map_diff = (int) TOKEN_AUGMENTED_ASSIGN_OPERATORS + 1;

    TypeExpressionOperation token_type_mapped_to_operation[SHORTEN_OPERATORS_COUNT];
    TypeInstruction token_type_mapped_to_instruction[SHORTEN_OPERATORS_COUNT];
    token_type_mapped_to_operation[TOKEN_ASSIGN_ADD - map_diff] = OPERATION_ADD;
    token_type_mapped_to_operation[TOKEN_ASSIGN_SUB - map_diff] = OPERATION_SUB;
    token_type_mapped_to_operation[TOKEN_ASSIGN_MULTIPLY - map_diff] = OPERATION_MULTIPLY;
    token_type_mapped_to_operation[TOKEN_ASSIGN_INT_DIVIDE - map_diff] = OPERATION_INT_DIVIDE;
    token_type_mapped_to_operation[TOKEN_ASSIGN_DIVIDE - map_diff] = OPERATION_DIVIDE;

    token_type_mapped_to_instruction[TOKEN_ASSIGN_ADD - map_diff] = I_ADD_STACK;
    token_type_mapped_to_instruction[TOKEN_ASSIGN_SUB - map_diff] = I_SUB_STACK;
    token_type_mapped_to_instruction[TOKEN_ASSIGN_MULTIPLY - map_diff] = I_MUL_STACK;
    token_type_mapped_to_instruction[TOKEN_ASSIGN_INT_DIVIDE - map_diff] = I_DIV_STACK;
    token_type_mapped_to_instruction[TOKEN_ASSIGN_DIVIDE - map_diff] = I_DIV_STACK;

    DataType expression_data_type;
    SymbolVariable* actual_variable = parser->parser_semantic->actual_variable;

    TypeExpressionOperation operation_type;
    TypeInstruction corresponding_instruction;

    RULES(
            CHECK_TOKEN(TOKEN_AUGMENTED_ASSIGN_OPERATORS);
            operation_type = token_type_mapped_to_operation[token_type - map_diff];
            corresponding_instruction = token_type_mapped_to_instruction[token_type - map_diff];

            CodeConstructor* constructor = parser->code_constructor;
            UNUSED(constructor);
            CodeInstruction* before_expression_instruction = NULL;
            CODE_GENERATION(
                 before_expression_instruction = code_generator_last_instruction(parser->code_constructor->generator);
            );
            CALL_EXPRESSION_RULE(expression_data_type);
            CODE_GENERATION(
                before_expression_instruction->next
                        ->meta_data.type |= CODE_INSTRUCTION_META_TYPE_EXPRESSION_START;
            );
    );

    SEMANTIC_ANALYSIS(
            {
                if(actual_variable == NULL)
                    return false;
                CHECK_BINARY_OPERATION_IMPLICIT_CONVERSION(
                        operation_type,
                        actual_variable->data_type,
                        expression_data_type
                );
            }
    );

    CODE_GENERATION(
            {
                OperationSignature* operation_signature = parser_semantic_get_operation_signature(
                        parser->parser_semantic,
                        operation_type,
                        actual_variable->data_type,
                        expression_data_type,
                        DATA_TYPE_ANY
                );

                CodeConstructor* constructor = parser->code_constructor;
                if(
                        operation_type == OPERATION_ADD &&
                        expression_data_type == DATA_TYPE_STRING &&
                        actual_variable->data_type == DATA_TYPE_STRING
                        ) {
                    // special case for string concat
                    GENERATE_CODE(
                            I_POP_STACK,
                            code_instruction_operand_init_variable(parser->parser_semantic->temp_variable1)
                    );
                    GENERATE_CODE(
                            I_CONCAT_STRING,
                            code_instruction_operand_init_variable(actual_variable),
                            code_instruction_operand_init_variable(actual_variable),
                            code_instruction_operand_init_variable(parser->parser_semantic->temp_variable1)
                    );
                } else {
                    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(
                            expression_data_type,
                            operation_signature->conversion_target_type
                    );
                    GENERATE_CODE(
                            I_POP_STACK,
                            code_instruction_operand_init_variable(parser->parser_semantic->temp_variable1)
                    );

                    GENERATE_CODE(
                            I_PUSH_STACK,
                            code_instruction_operand_init_variable(actual_variable)
                    );
                    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(
                            actual_variable->data_type,
                            operation_signature->conversion_target_type
                    );
                    GENERATE_CODE(
                            I_PUSH_STACK,
                            code_instruction_operand_init_variable(parser->parser_semantic->temp_variable1)
                    );

                    GENERATE_CODE(corresponding_instruction);
                    GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(
                            operation_signature->result_type,
                            actual_variable->data_type
                    );
                    GENERATE_CODE(I_POP_STACK, code_instruction_operand_init_variable(actual_variable));
                    code_generator_last_instruction(parser->code_constructor->generator)
                        ->meta_data.type |= CODE_INSTRUCTION_META_TYPE_EXPRESSION_END;
                }
            }
    );

    parser->parser_semantic->actual_variable = NULL;
    return true;
}
#undef SHORTEN_OPERATORS_COUNT
