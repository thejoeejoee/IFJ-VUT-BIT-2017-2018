#include "data_type.h"

bool operands_match_data_type_combination(DataType first_operand, DataType second_operand,
                                          DataType expected_operand_data_type_1,
                                          DataType expected_operand_data_type_2) {
    return ((first_operand == expected_operand_data_type_1 &&
             second_operand == expected_operand_data_type_2) ||
            (first_operand == expected_operand_data_type_2 &&
             second_operand == expected_operand_data_type_1));
}
