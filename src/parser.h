#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>
#include "lexer.h"
#include "lexer_fsm.h"
#include "parser_semantic.h"
#include "memory.h"
#include "error.h"

//Todo: we need to invent better macros
#define GET_NEXT_TOKEN_TYPE()\
    token = lexer_next_token(parser->lexer);\
    token_type = token->type;\
    if (token_type == TOKEN_ERROR) \
        parser->error_report.error_code = ERROR_LEXER;

#define INIT_LOCAL_TOKEN_VARS() Token *token; TokenType token_type;

#define CALL_RULE(Rule) if (!parser_parse_##Rule(parser)) return false;

#define TEST_TOKEN_TYPE(Type) if(token_type != (Type)) return false;

#define TEST_TOKEN_IS_DATA_TYPE() if(token_type != TOKEN_INTEGER && token_type != TOKEN_STRING && token_type != TOKEN_DOUBLE) return false;

#define SEMANTIC_ANALYSIS(parser, code) do {\
if ((parser)->enabled_semantic_analysis) \
    code \
} while (0)

#define CODE_GENERATION(parser, code) do {\
if ((parser)->enabled_semantic_analysis) \
    code \
} while (0)


/**
 * @brief Representation of Parser
 */
typedef struct parser_t {
    Lexer* lexer;
    ParserSemantic* parser_semantic;
    ErrorReport error_report;

    // TODO: refactor to bit flags
    bool enabled_semantic_analysis;
    bool enabled_code_generation;
} Parser;

/**
 * @brief Constructor for parser
 *
 * @param input_stream lexer_input_stream_f Strem of input chars
 * @return Parser* Pointer to parser
 */
Parser* parser_init(lexer_input_stream_f input_stream);

/**
 * @brief Constructor for parser
 *
 * @param Parser** parser Pointer to pointer to instance of Parser
 * @return Parser* Pointer to parser
 */
void parser_free(Parser** parser);

/**
 * @brief
 *
 * @param parser
 * @return
 */
bool parser_parse(Parser* parser);

/**
 * @brief Performs <prog> of the program
 * @return bool true If the parsing performed successfully, false if not
 * @param parser Parser* Pointer to instance of parser
 *
 * Persorms rule with shape
 *
 * <prog> -> <program> EOF
 */
bool parser_parse_program(Parser* parser);

/**
 * @brief Performs <body> rule
 * @return bool true If the parsing performed successfully, false if not
 * @param parser Parser* Pointer to instance of parser
 *
 * Persorms rule with shape
 *
 * <body> -> <definitions> SCOPE <statements> END SCOPE
 */
bool parser_parse_body(Parser* parser);

/**
 * @brief <definitions> is EPSIOLON, it will be implemented in the future
 */
bool parser_parse_definitions(Parser* parser);

/**
 * @brief Performs <fucntion_param> rule
 * @return bool true If the parsing performed successfully, false if not
 * @param parser Parser* Pointer to instance of parser
 *
 * Persorms rule with shape
 *
 * <function_param> -> <id> AS <type>
 */
bool parser_parse_function_param(Parser* parser);

/**
 * @brief Performs <fucntion_declaration> rule
 * @return bool true If the parsing performed successfully, false if not
 * @param parser Parser* Pointer to instance of parser
 *
 * Persorms rule with shape
 *
 * <function_declaration> -> DECLARE <function_header> EOL <eols>
 */
bool parser_parse_function_declaration(Parser* parser);


//Todo: comments

bool parser_parse_function_statements(Parser* parser);

bool parser_parse_body_statements(Parser* parser);

bool parser_parse_function_header(Parser* parser);

bool parser_parse_eols(Parser* parser);

bool parser_parse_function_params(Parser* parser);

bool parser_parse_function_n_param(Parser* parser);

bool parser_parse_definition(Parser* parser);

bool parser_parse_function_definition(Parser* parser);

bool parser_parse_function_statement_single(Parser* parser);

bool parser_parse_body_statement_single(Parser* parser);


#endif //_PARSER_H
