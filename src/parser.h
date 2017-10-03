#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>
#include "lexer.h"
#include "lexer_fsm.h"
#include "parser_semantic.h"
#include "memory.h"
#include "error.h"
#include "common.h"

//Todo: we need to invent better macros
#define GET_NEXT_TOKEN_TYPE() do { \
    token = lexer_next_token(parser->lexer); \
    token_type = token->type; \
    if (token_type == TOKEN_ERROR) { \
        parser->error_report.error_code = ERROR_LEXER; \
        parser->error_report.detail_information = (int )parser->lexer->lexer_fsm->lexer_error; \
    }} while(false);


#define INIT_LOCAL_TOKEN_VARS() Token *token; TokenType token_type; (void) token; (void) token_type;

#define CALL_RULE(Rule) if (!parser_parse_##Rule(parser)) return false;

#define TEST_TOKEN_TYPE(Type) if(token_type != (Type)) return false;

#define TEST_TOKEN_IS_DATA_TYPE() if(token_type != TOKEN_INTEGER && token_type != TOKEN_STRING && token_type != TOKEN_DOUBLE) return false;




// NEW MACROS
#define BEFORE(code) do { code } while(false);
#define AFTER(code) do { code } while(false);
#define NO_CODE BEFORE(), AFTER()

#define RULES(rules) \
    do { \
        Token* token = NULL; \
        TokenType token_type = TOKEN_UNKNOWN; \
        bool conditional_rules = false; \
        unsigned int conditions_buffer = 0; \
        rules \
    } while(false)

#define CONDITIONAL_RULES(rules) do { \
        GET_NEXT_TOKEN_TYPE(); \
        conditions_buffer = 0; \
        conditional_rules = true; \
        rules \
        conditional_rules = false; \
    } while(false)

#define CHECK_RULE_4(condition, rule_name, before_code, after_code) do { \
        if(conditional_rules) { \
            if(condition) { \
                conditions_buffer <<= 1; \
                conditions_buffer |= 1; \
                before_code \
                if(!parser_parse_ ## rule_name(parser)) return false; \
                after_code \
            } \
        }   \
        else { \
            before_code \
            if(!parser_parse_ ## rule_name(parser)) return false; \
            after_code \
        } \
    } while(false)

#define CHECK_RULE_3(rule_name, before_code, after_code) do { \
    if(conditional_rules) { \
        if(conditions_buffer == 0) { \
            before_code \
            if(!parser_parse_ ## rule_name(parser)) return false; \
            after_code \
        } \
    }   \
    else { \
        before_code \
        if(!parser_parse_ ## rule_name(parser)) return false; \
        after_code \
    } \
} while(false)

#define CHECK_RULE_2(rule_name, before_code) CHECK_RULE_3(rule_name, before_code, AFTER())
#define CHECK_RULE_1(rule_name) CHECK_RULE_3(rule_name, BEFORE(), AFTER())

#define CHECK_RULE(...) MSVC_EXPAND(GET_OVERLOADED_MACRO34( \
    __VA_ARGS__, CHECK_RULE_4, CHECK_RULE_3, CHECK_RULE_2, CHECK_RULE_1)(__VA_ARGS__))

#define _RAW_CHECK_TOKEN(token_type_literal) \
    if(((token_type & (token_type_literal)) == 0 && (token_type_literal) >= TOKEN_CLASSES &&  \
    (token_type_literal & 0xFF) == 0) || \
    (token_type != (token_type_literal) && (token_type_literal) < TOKEN_CLASSES)) \
        return false

#define CHECK_TOKEN_4(condition, token_type_literal, before_code, after_code) do { \
        if(conditional_rules) { \
            if(condition) { \
                before_code \
                conditions_buffer <<= 1; \
                conditions_buffer |= 1; \
                GET_NEXT_TOKEN_TYPE(); \
                _RAW_CHECK_TOKEN(token_type_literal); \
                after_code \
            } \
        } \
        else { \
            before_code \
            GET_NEXT_TOKEN_TYPE(); \
            _RAW_CHECK_TOKEN(token_type_literal); \
            after_code \
        }\
    } while(false)

#define CHECK_TOKEN_3(token_type_literal, before_code, after_code) do { \
    if(conditional_rules) { \
        if(conditions_buffer == 0) { \
            before_code \
            GET_NEXT_TOKEN_TYPE(); \
            _RAW_CHECK_TOKEN(token_type_literal); \
            after_code \
        } \
    } \
    else { \
        before_code \
        GET_NEXT_TOKEN_TYPE(); \
        _RAW_CHECK_TOKEN(token_type_literal); \
        after_code \
    }\
} while(false)

#define CHECK_TOKEN_2(token_type_literal, before_code) CHECK_TOKEN_3( \
    token_type_literal, before_code, AFTER())
#define CHECK_TOKEN_1(token_type_literal) CHECK_TOKEN_3(token_type_literal, BEFORE(), AFTER())

#define CHECK_TOKEN(...) MSVC_EXPAND(GET_OVERLOADED_MACRO34(\
    __VA_ARGS__, CHECK_TOKEN_4, CHECK_TOKEN_3, CHECK_TOKEN_2, CHECK_TOKEN_1)(__VA_ARGS__))

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

bool parser_parse_epsilon(Parser* parser);


#endif //_PARSER_H
