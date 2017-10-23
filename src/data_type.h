#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H

#include "token.h"

typedef enum {
    DATA_TYPE_NONE = 0,
    DATA_TYPE_INTEGER = TOKEN_INTEGER,
    DATA_TYPE_DOUBLE = TOKEN_DOUBLE,
    DATA_TYPE_STRING = TOKEN_STRING,
    DATA_TYPE_BOOLEAN, // TODO: assign to one of tokens
} DataType;

// TODO doc and test
bool operands_match_data_type_combination(DataType first_operand, DataType second_operand, DataType expected_operand_data_type_1, DataType expected_operand_data_type_2);

#endif //_DATA_TYPE_H
