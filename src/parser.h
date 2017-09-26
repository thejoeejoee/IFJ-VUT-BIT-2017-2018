
#ifndef IFJ2017_PARSER_H
#define IFJ2017_PARSER_H

#include <stdbool.h>
#include "lexer.h"
#include "lexer_fsm.h"
#include "memory.h"


/**
 * @brief Representation of Parser
 */
typedef struct parser_t {
    Lexer *lexer;
} Parser;

/**
 * @brief Constructor for parser
 *
 * @param input_stream lexer_input_stream_f Strem of input chars
 * @return Parser* Pointer to parser
 */
Parser *parser_init(lexer_input_stream_f input_stream);

/**
 * @brief Constructor for parser
 *
 * @param Parser** parser Pointer to pointer to instance of Parser
 * @return Parser* Pointer to parser
 */
void parser_free(Parser **parser);

/**
 * @brief Performs <prog> of the program
 * @return bool true If the parsing performed successfully, false if not
 * @param parser Parser* Pointer to instance of parser
 *
 * Persorms rule with shape
 *
 * <prog> -> <program> EOF
 */

bool parser_parse_program(Parser *parser);

/**
 * @brief Performs <body> rule
 * @return bool true If the parsing performed successfully, false if not
 * @param parser Parser* Pointer to instance of parser
 *
 * Persorms rule with shape
 *
 * <body> -> <definitions> SCOPE <statements> END SCOPE
 */
bool parser_parse_body(Parser *parser);

/**
 * @brief <definitions> is EPSIOLON, it will be implemented in the future
 */
bool parser_parse_definitions(Parser *parser);

/**
 * @brief <statements> is EPSIOLON, it will be implemented in the future
 */
bool parser_parse_statements(Parser *parser);

/**
 * @brief Performs <fucntion_param> rule
 * @return bool true If the parsing performed successfully, false if not
 * @param parser Parser* Pointer to instance of parser
 *
 * Persorms rule with shape
 *
 * <function_param> -> <id> AS <type>
 */
bool parser_function_param(Parser *parser);


#endif //IFJ2017_PARSER_H
