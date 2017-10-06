#include "gtest/gtest.h"

extern "C" {
#include "../src/code_instruction_operand.h"
#include "../src/code_instruction.h"
}

class CodeInstructionOperandTestFixture : public ::testing::Test {
    protected:
        CodeInstructionOperand* operand;

        virtual void TearDown() {
            code_instruction_operand_free(&operand);
        }
};

TEST_F(CodeInstructionOperandTestFixture, Label) {

    auto label = const_cast<char*>("label ");
    operand = code_instruction_operand_init_label(label);

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_LABEL
    );
    EXPECT_EQ(
            operand->data.label,
            label
    );
}


TEST_F(CodeInstructionOperandTestFixture, Integer) {

    auto value = 142;
    operand = code_instruction_operand_init_integer(value);

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data.integer,
            value
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_INTEGER
    );
}

TEST_F(CodeInstructionOperandTestFixture, Double) {

    auto value = 142.78;
    operand = code_instruction_operand_init_double(value);

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data.double_,
            value
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_DOUBLE
    );
}

TEST_F(CodeInstructionOperandTestFixture, Boolean) {
    auto value = false;
    operand = code_instruction_operand_init_boolean(value);

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data.boolean,
            value
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_BOOLEAN
    );
}
