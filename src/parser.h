#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>
#include "lexer.h"
#include "lexer_fsm.h"
#include "parser_semantic.h"
#include "memory.h"
#include "error.h"
#include "common.h"
#include "token.h"
#include "code_constructor.h"

//Todo: we need to invent better macros
#define GET_NEXT_TOKEN_TYPE() do { \
    token_free(&token);\
    token = lexer_next_token(parser->lexer); \
    token_type = token.type; \
    } while(0);


#define CALL_RULE(Rule) if (!parser_parse_##Rule(parser)) { token_free(&token); return false; }
#define CALL_EXPRESSION_RULE(data_type_var) do {if (!parser_parse_expression(parser, &data_type_var)) { token_free(&token); return false; }} while(false)

// NEW MACROS
#define BEFORE(code) do {code} while(false);
#define AFTER(code) do {code} while(false);
#define NO_CODE {},{}
#define REWIND_AND_SUCCESS BEFORE({lexer_rewind_token(parser->lexer, token);}), AFTER({token_free(&token); return true;})

#define RULES(rules) \
    do { \
        Token token; \
        token.data = NULL; \
        TokenType token_type = TOKEN_UNKNOWN; UNUSED(token_type); \
        bool conditional_rules = false; \
        unsigned int conditions_buffer = 0; \
        rules \
        token_free(&token); \
    } while(0)

#define CONDITIONAL_RULES(rules) do { \
        GET_NEXT_TOKEN_TYPE(); \
        conditions_buffer = 0; \
        conditional_rules = true; \
        rules \
        conditional_rules = false; \
    } while(0)

#define CHECK_RULE_4(condition, rule_name, before_code, after_code) do { \
        if(conditional_rules) { \
            if(condition) { \
                conditions_buffer <<= 1; \
                conditions_buffer |= 1; \
                before_code \
                _RAW_CHECK_RULE(rule_name); \
                after_code \
            } \
        }   \
        else { \
            before_code \
            _RAW_CHECK_RULE(rule_name); \
            after_code \
        } \
    } while(0)

#define CHECK_RULE_3(rule_name, before_code, after_code) do { \
    if(conditional_rules) { \
        if(conditions_buffer == 0) { \
            before_code \
            _RAW_CHECK_RULE(rule_name); \
            after_code \
        } \
    }   \
    else { \
        before_code \
        _RAW_CHECK_RULE(rule_name); \
        after_code \
    } \
} while(0)

#define CHECK_RULE_2(rule_name, before_code) CHECK_RULE_3(rule_name, before_code, ;)
#define CHECK_RULE_1(rule_name) CHECK_RULE_3(rule_name, ;, ;)

#define CHECK_RULE(...) MSVC_EXPAND(GET_OVERLOADED_MACRO1234( \
    __VA_ARGS__, CHECK_RULE_4, CHECK_RULE_3, CHECK_RULE_2, CHECK_RULE_1)(__VA_ARGS__))

#define _RAW_CHECK_TOKEN(token_type_literal) do { \
    if(!token_check(token,(token_type_literal))) {\
        token_free(&token); \
        LOG_INFO("Token test %s fail, %d present.", #token_type_literal, token_type); \
        return false; \
    } \
    LOG_INFO("Token test %s success.", #token_type_literal); \
    } while(0)

#define _RAW_CHECK_RULE(rule_name) do { \
    if(!parser_parse_ ## rule_name(parser)) {\
        token_free(&token);\
        LOG_INFO("Rule test %s fail.", #rule_name); \
        return false;\
    } \
    LOG_INFO("Rule test %s success.", #rule_name); \
    } while(0)

#define CHECK_TOKEN_4(condition, token_type_literal, before_code, after_code) do { \
        if(conditional_rules) { \
            if(condition) { \
                conditions_buffer <<= 1; \
                conditions_buffer |= 1; \
                GET_NEXT_TOKEN_TYPE(); \
                before_code \
                _RAW_CHECK_TOKEN(token_type_literal); \
                after_code \
            } \
        } \
        else { \
            GET_NEXT_TOKEN_TYPE(); \
            before_code \
            _RAW_CHECK_TOKEN(token_type_literal); \
            after_code \
        }\
    } while(0)

#define CHECK_TOKEN_3(token_type_literal, before_code, after_code) do { \
    if(conditional_rules) { \
        if(conditions_buffer == 0) { \
            GET_NEXT_TOKEN_TYPE(); \
            before_code \
            _RAW_CHECK_TOKEN(token_type_literal); \
            after_code \
        } \
    } \
    else { \
        GET_NEXT_TOKEN_TYPE(); \
        before_code \
        _RAW_CHECK_TOKEN(token_type_literal); \
        after_code \
    }\
} while(0)

#define CHECK_TOKEN_2(token_type_literal, before_code) CHECK_TOKEN_3( \
    token_type_literal, before_code, ;)
#define CHECK_TOKEN_1(token_type_literal) CHECK_TOKEN_3(token_type_literal, ;, ;)

#define CHECK_TOKEN(...) MSVC_EXPAND(GET_OVERLOADED_MACRO1234(\
    __VA_ARGS__, CHECK_TOKEN_4, CHECK_TOKEN_3, CHECK_TOKEN_2, CHECK_TOKEN_1)(__VA_ARGS__))

#define SEMANTIC_ANALYSIS(code) do { \
if ((parser)->run_type & PARSER_RUN_TYPE_SEMANTIC_ANALYSIS) { \
    code \
}} while(0)

#define CODE_GENERATION(code) do { \
if ((parser)->run_type & PARSER_RUN_TYPE_SEMANTIC_CODE_GENERATION) { \
    code \
}} while(0)

#define CALL_RULE_STATEMENTS() do { \
if(parser->body_statement) { \
    CALL_RULE(body_statements); \
} else { \
    CALL_RULE(function_statements); \
}} while(0)

typedef enum {
    PARSER_RUN_TYPE_NOTHING = 0,
    PARSER_RUN_TYPE_SEMANTIC_ANALYSIS = 1,
    PARSER_RUN_TYPE_SEMANTIC_CODE_GENERATION = 2,
    PARSER_RUN_TYPE_ALL = PARSER_RUN_TYPE_SEMANTIC_ANALYSIS | PARSER_RUN_TYPE_SEMANTIC_CODE_GENERATION,
} ParserRunType;

/**
 * @brief Representation of Parser
 */
typedef struct parser_t {
    Lexer* lexer;
    ParserSemantic* parser_semantic;
    ErrorReport error_report;
    CodeConstructor* code_constructor;

    int run_type;

    bool body_statement; // TODO: cannot by done with semantic parser action?

    /** GET_NEXT_TOKEN_TYPE()
    * Token token;
    * TokenType token_type;
    */
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

bool parser_parse_epsilon(Parser* parser);

bool parser_parse_variable_declaration(Parser* parser);

bool parser_parse_return_(Parser* parser);

bool parser_parse_input(Parser* parser);

bool parser_parse_print_expression(Parser* parser);

bool parser_parse_print(Parser* parser);

bool parser_parse_while_(Parser* parser);

bool parser_parse_print_expressions(Parser* parser);

bool parser_parse_condition(Parser* parser);

bool parser_parse_condition_else(Parser* parser);

bool parser_parse_condition_elseif(Parser* parser);

bool parser_parse_scope(Parser* parser);

bool parser_parse_assignment(Parser* parser);

bool parser_parse_declaration_assignment(Parser* parser);

bool parser_parse_identifier_assignment(Parser* parser);

#endif //_PARSER_H
