#include "parser.h"
#include "lexer.h"
#include "lexer_fsm.h"
#include "token.h"

//Todo: we need to invent better macros

#define GET_NEXT_TOKEN_TYPE() token = lexer_next_token(parser->lexer); token_type = token->type; memory_free(token);

#define INIT_LOCAL_TOKEN_VARS() Token *token; TokenType token_type;

#define CALL_RULE(Rule) if (!parser_parse_##Rule(parser)) return false;

#define TEST_TOKEN_TYPE(Type) if(token_type != Type) return false;

#define TEST_TOKEN_IS_DATA_TYPE() if(token_type != TOKEN_INTEGER && token_type != TOKEN_STRING && token_type != TOKEN_DOUBLE) return false;


Parser *parser_init(lexer_input_stream_f input_stream) {
    Parser* parser = (Parser*) memory_alloc(sizeof(Parser));

    NULL_POINTER_CHECK(parser, NULL);

    parser->lexer = lexer_init(input_stream);
    return parser;
}

void parser_free(Parser **parser) {

    lexer_free(&((*parser)->lexer));
    memory_free(*parser);
    *parser = NULL;
}

bool parser_parse_program(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS();
    /*
     * RULE
     * <prog> -> <body> EOF
     */

    // Call rule <body>. If <body> return false => return false
    CALL_RULE(body);


    // Expect EOF token If return true, program is syntactically correct
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_EOF);

    return true;

}

bool parser_parse_body(Parser* parser) {

    INIT_LOCAL_TOKEN_VARS()

    /**
     * RULE
     * <body> -> <definitions> SCOPE <statements> END SCOPE
     */

    // Call <definitions> rule
    CALL_RULE(definitions);

    // Expect SCOPE token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_SCOPE);

    // Call <statements> rule
    CALL_RULE(statements);

    // Expect END token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_END);

    // Expect SCOPE token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_SCOPE);

    return true;
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
    token_type = token->type;
    lexer_return_token(parser->lexer, token);
    if(token_type == TOKEN_RIGHT_BRACKET) {
        // It is EPSILON

        return true;
    }
    else {
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
    token_type = token->type;

    if(token_type == TOKEN_RIGHT_BRACKET) {
        // It is EPSILON
        lexer_return_token(parser->lexer, token);
        return true;
    }
    else {
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

    // Expect AS token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_TYPE(TOKEN_AS);

    // Expect TYPE token
    GET_NEXT_TOKEN_TYPE();
    TEST_TOKEN_IS_DATA_TYPE()

    return true;

}


bool parser_parse_eols(Parser* parser) {

    //Todo: comment rules

    INIT_LOCAL_TOKEN_VARS()

    GET_NEXT_TOKEN_TYPE();
    if (token_type == TOKEN_EOL) {
        CALL_RULE(eols)
    }
    else
        lexer_return_token(parser->lexer, token);

    return true;
}

bool parser_parse_definitions(Parser* parser) {

    // Todo: It is epsilon, it will be implemented in the future
    return true;
}

bool parser_parse_statements(Parser* parser) {

    // Todo: It is epsilon, it will be implemented in the future
    return true;
}



