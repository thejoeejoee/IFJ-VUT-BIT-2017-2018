#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H

#include "token.h"

#define CHECK_VALID_DATA_TYPE(data_type) do { \
    if ((data_type) == DATA_TYPE_NONE || (data_type) == DATA_TYPE_ANY) { \
        LOG_WARNING("Invalid data type for " #data_type ": %d.", data_type); \
    } \
} while(0)


typedef enum {
    DATA_TYPE_ANY = -1,
    DATA_TYPE_NONE = 0,
    DATA_TYPE_INTEGER = TOKEN_INTEGER,
    DATA_TYPE_DOUBLE = TOKEN_DOUBLE,
    DATA_TYPE_STRING = TOKEN_STRING,
    DATA_TYPE_BOOLEAN = TOKEN_BOOLEAN
} DataType;

/**
 * Checks two operands with expected data types and returns true, if both combinations are valid.
 * @param first_operand
 * @param second_operand
 * @param expected_operand_data_type_1
 * @param expected_operand_data_type_2
 * @return
 */
bool operands_match_data_type_combination(DataType first_operand, DataType second_operand,
                                          DataType expected_operand_data_type_1, DataType expected_operand_data_type_2);

#endif //_DATA_TYPE_H
