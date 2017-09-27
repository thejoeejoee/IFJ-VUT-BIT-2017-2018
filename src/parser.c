//
// Created by martin on 26.9.17.
//

#include "parser.h"
#include "lexer.h"
#include "lexer_fsm.h"
#include "token.h"

Parser* parser_init(lexer_input_stream_f input_stream) {
    Parser* parser = (Parser*) memory_alloc(sizeof(Parser));

    NULL_POINTER_CHECK(parser, NULL);

    parser->lexer = lexer_init(input_stream);
    return parser;
}

void parser_free(Parser** parser) {

    lexer_free(&((*parser)->lexer));
    memory_free(*parser);
    *parser = NULL;
}

bool parser_parse_program(Parser* parser) {
    Token* token;
    TokenType token_type;

    /*
     * RULE
     * <prog> -> <body> EOF
     */

    // Call rule <body>. If <body> return false => return false
    if(!parser_parse_body(parser))
        return false;

    // Expect EOF token
    token = lexer_next_token(parser->lexer);
    token_type = token->type;
    memory_free(token);

    // If return true, program is syntactically correct
    return (token_type == TOKEN_EOF);

}

bool parser_parse_body(Parser* parser) {
    Token* token;
    TokenType token_type;

    /**
     * RULE
     * <body> -> <definitions> SCOPE <statements> END SCOPE
     */

    // Call <definitions> rule
    if(!parser_parse_definitions(parser))
        return false;

    // Expect SCOPE token
    token = lexer_next_token(parser->lexer);
    token_type = token->type;
    memory_free(token);
    if(token_type != TOKEN_SCOPE)
        return false;

    // Call <statements> rule
    if(!parser_parse_statements(parser))
        return false;

    // Expect END token
    token = lexer_next_token(parser->lexer);
    token_type = token->type;
    if(token_type != TOKEN_END)
        return false;

    // Expect SCOPE token
    token = lexer_next_token(parser->lexer);
    token_type = token->type;
    return (token_type == TOKEN_SCOPE);
}

bool parser_parse_definitions(Parser* parser) {

    // Todo: It is epsilon, it will be implemented in the future
    return true;
}

bool parser_parse_statements(Parser* parser) {

    // Todo: It is epsilon, it will be implemented in the future
    return true;
}

bool parser_function_param(Parser* parser) {
    Token* token;
    TokenType token_type;

    /**
     * RULE
     * <function_param> -> IDENTIFIER AS TYPE
     */

    // Expect IDENTIFIER token
    token = lexer_next_token(parser->lexer);
    token_type = token->type;
    memory_free(token);
    if(token_type != TOKEN_IDENTIFIER)
        return false;

    // Expect AS token
    token = lexer_next_token(parser->lexer);
    token_type = token->type;
    memory_free(token);
    if(token_type != TOKEN_AS)
        return false;

    // Expect TYPE token
    token = lexer_next_token(parser->lexer);
    token_type = token->type;
    memory_free(token);
    return (token_type == TOKEN_INTEGER || token_type == TOKEN_STRING || token_type == TOKEN_DOUBLE);

}