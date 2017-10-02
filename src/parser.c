#include "parser.h"
#include "parser_semantic.h"
#include "lexer.h"
#include "lexer_fsm.h"
#include "token.h"


Parser* parser_init(lexer_input_stream_f input_stream) {
    Parser* parser = (Parser*) memory_alloc(sizeof(Parser));

    parser->lexer = lexer_init(input_stream);
    parser->error_report.error_code = ERROR_NONE;
    parser->parser_semantic = parser_semantic_init();
    parser->enabled_code_generation = parser->enabled_semantic_analysis = true;
    return parser;
}

void parser_free(Parser** parser) {

    lexer_free(&((*parser)->lexer));
    parser_semantic_free(&((*parser)->parser_semantic));
    memory_free(*parser);
    *parser = NULL;
}

bool parser_parse(Parser* parser) {

    if(!parser_parse_program(parser)) {
        parser->error_report.line = parser->lexer->lexer_fsm->line;

        if(parser->error_report.error_code == ERROR_NONE)
            parser->error_report.error_code = ERROR_SYNTAX;

        return false;
    }

    return true;

}

bool parser_parse_program(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS();

    /*
     * RULE
     * <prog> -> <body> <eols> EOF
     */

    // Call rule <body>. If <body> return false => return false
    CALL_RULE(body);

    CALL_RULE(eols);

    // Expect EOF token If return true, program is syntactically correct
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_EOF);

    return true;
}

bool parser_parse_body(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * <body> -> <definitions> SCOPE EOL <statements> END SCOPE
     */

    // Call <definitions> rule
    CALL_RULE(definitions);

    // Expect SCOPE token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_SCOPE);

    // Expect EOL token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_EOL);

    // Call <statements> rule
    CALL_RULE(body_statements);

    // Expect END token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_END);

    // Expect SCOPE token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_SCOPE);

    return true;
}

bool parser_parse_definitions(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULES
     *
     * <definitions> -> <eols> <definition> <definitions>
     * <definitions> -> <eols> E
     */

    CALL_RULE(eols)

    token = lexer_next_token(parser->lexer);
    token_type = token.type;
    if(token_type != TOKEN_DECLARE && token_type != TOKEN_FUNCTION)
        // Epsilon
        lexer_rewind_token(parser->lexer, token);
    else {
        lexer_rewind_token(parser->lexer, token);
        CALL_RULE(definition)
        CALL_RULE(definitions)
    }

    return true;
}

bool parser_parse_definition(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULES
     * <definition> -> <function_declaration>
     * <definition> -> <function_definition>
     */

    token = lexer_next_token(parser->lexer);
    token_type = token.type;
    if(token_type == TOKEN_DECLARE) {
        lexer_rewind_token(parser->lexer, token);
        CALL_RULE(function_declaration)
    } else if(token_type == TOKEN_FUNCTION) {
        lexer_rewind_token(parser->lexer, token);
        CALL_RULE(function_definition)
    } else
        // Zatím Epsilon
        lexer_rewind_token(parser->lexer, token);
    return true;
}

bool parser_parse_function_definition(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * <function_definition> -> <function_header> EOL <eols> <statements> END FUNCTION
     */
    CALL_RULE(function_header)
    GET_NEXT_TOKEN_TYPE()
    TEST_TOKEN_TYPE(TOKEN_EOL)
    CALL_RULE(eols)

    CALL_RULE(function_statements)

    GET_NEXT_TOKEN_TYPE()
    TEST_TOKEN_TYPE(TOKEN_END)
    GET_NEXT_TOKEN_TYPE()
    TEST_TOKEN_TYPE(TOKEN_FUNCTION)
    return true;
}

bool parser_parse_function_statements(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULES
     * <statements> -> E
     * <statements> -> <statement_single> EOL <eols> <statements>
     */

    token = lexer_next_token(parser->lexer);
    token_type = token.type;
    lexer_rewind_token(parser->lexer, token);

    if(token_type != TOKEN_INPUT) {

        // It is EPSILON
        return true;
    } else {
        CALL_RULE(function_statement_single)
        GET_NEXT_TOKEN_TYPE()
        TEST_TOKEN_TYPE(TOKEN_EOL)
        CALL_RULE(eols)
        CALL_RULE(function_statements)
    }

    return true;
}

bool parser_parse_body_statements(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULES
     * <statements> -> E
     * <statements> -> <statement_single> EOL <eols> <statements>
     */

    token = lexer_next_token(parser->lexer);
    token_type = token.type;
    lexer_rewind_token(parser->lexer, token);

    if(token_type != TOKEN_INPUT) {

        // It is EPSILON
        return true;
    } else {
        CALL_RULE(body_statement_single)
        GET_NEXT_TOKEN_TYPE()
        TEST_TOKEN_TYPE(TOKEN_EOL)
        CALL_RULE(eols)
        CALL_RULE(body_statements)
    }

    return true;
}

bool parser_parse_function_statement_single(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * <statement_single> -> INPUT <id>
     */

    token = lexer_next_token(parser->lexer);
    token_type = token.type;

    if(token_type == TOKEN_INPUT) {

        GET_NEXT_TOKEN_TYPE()
        TEST_TOKEN_TYPE(TOKEN_IDENTIFIER)

        FREE_TOKEN_DATA();
        return true;
    }

    return false;
}

bool parser_parse_body_statement_single(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * <statement_single> -> INPUT <id>
     */

    token = lexer_next_token(parser->lexer);
    token_type = token.type;

    if(token_type == TOKEN_INPUT) {

        GET_NEXT_TOKEN_TYPE()
        TEST_TOKEN_TYPE(TOKEN_IDENTIFIER)
        SEMANTIC_ANALYSIS(
                parser,
                if(NULL == symbol_register_find_variable_recursive(parser->parser_semantic->register_, token.data)) {
                    FREE_TOKEN_DATA();
                    return false;
                }
        );
        FREE_TOKEN_DATA();
        return true;
    }
    // TODO: where to free token data?
    if(token.data != NULL)
        memory_free(token.data);

    return false;
}


bool parser_parse_function_declaration(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * <function_declaration> -> DECLARE <function_header> EOL <eols>
     */

    // Expect SCOPE token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_DECLARE);

    // Call <statements> rule
    CALL_RULE(function_header)

    // Expect EOL token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_EOL);

    // Call <eols> rule
    CALL_RULE(eols)

    return true;
}

bool parser_parse_function_header(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * FUNCTION IDENTIFIER (<function_params>) AS <type>
     */

    // Expect FUNCTION token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_FUNCTION);

    // Expect IDENTIFIER token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_IDENTIFIER);
    FREE_TOKEN_DATA();

    // Expect LEFT_BRACKET token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_LEFT_BRACKET);

    // Call <function_params> rule
    CALL_RULE(function_params)

    // Expect RIGHT_BRACKET token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_RIGHT_BRACKET);

    // Expect AS token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_AS);

    // Expect <type>
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_IS_DATA_TYPE()

    return true;
}

bool parser_parse_function_params(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * E
     * <function_param> <function_n_param>
     */

    token = lexer_next_token(parser->lexer);
    token_type = token.type;
    lexer_rewind_token(parser->lexer, token);

    if(token_type == TOKEN_RIGHT_BRACKET) {
        // It is EPSILON

        return true;
    } else {
        CALL_RULE(function_param)
        CALL_RULE(function_n_param)
    }

    return true;
}

bool parser_parse_function_n_param(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * E
     * <function_param> <function_n_param>
     */

    token = lexer_next_token(parser->lexer);
    token_type = token.type;

    if(token_type == TOKEN_RIGHT_BRACKET) {
        // It is EPSILON
        lexer_rewind_token(parser->lexer, token);
        return true;
    } else {
        CALL_RULE(function_param)
        CALL_RULE(function_n_param)
    }

    return true;
}

bool parser_parse_function_param(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * <function_param> -> IDENTIFIER AS TYPE
     */

    // Expect IDENTIFIER token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_IDENTIFIER);
    FREE_TOKEN_DATA();

    // Expect AS token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_AS);

    // Expect TYPE token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_IS_DATA_TYPE()

    return true;

}

bool parser_parse_eols(Parser* parser) {

    /**
     * RULES
     * <eols> -> E
     * <eols> -> EOL <eols>
     */

    INIT_LOCAL_TOKEN_VARS()

    token = lexer_next_token(parser->lexer);
    token_type = token.type;
    if(token_type == TOKEN_EOL) {
        CALL_RULE(eols)
    } else
        lexer_rewind_token(parser->lexer, token);

    return true;
}