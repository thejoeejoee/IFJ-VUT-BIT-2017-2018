#include "gtest/gtest.h"

extern "C" {
#include "../src/code_generator.h"
}

class CodeGeneratorTestFixture : public ::testing::Test {
    protected:
        CodeGenerator* code_generator;

        virtual void SetUp() {
            code_generator = code_generator_init();
        }

        virtual void TearDown() {
            code_generator_free(&code_generator);
        }
};

TEST_F(CodeGeneratorTestFixture, OneInstruction) {
    EXPECT_EQ(
            code_generator->first,
            nullptr
    );
    EXPECT_EQ(
            code_generator->last,
            nullptr
    );

    code_generate_I_DEF_VAR(code_generator);

    EXPECT_EQ(
            code_generator->first,
            code_generator->last
    ) << "One added instruction";
    EXPECT_EQ(
            code_generator->first->type,
            I_DEF_VAR
    );
    EXPECT_EQ(
            code_generator->first->op0,
            nullptr
    );
    EXPECT_EQ(
            code_generator->first->op1,
            nullptr
    );
    EXPECT_EQ(
            code_generator->first->op2,
            nullptr
    );
}


TEST_F(CodeGeneratorTestFixture, OneInstructionWithOperands) {
    CodeInstructionOperand* label_operand = code_instruction_operand_init_label("test");

    code_generate_I_JUMP(code_generator, label_operand);

    EXPECT_EQ(
            code_generator->first->op0,
            label_operand
    );
    EXPECT_EQ(
            code_generator->first->op1,
            nullptr
    );
    EXPECT_EQ(
            code_generator->first->op2,
            nullptr
    );
}


TEST_F(CodeGeneratorTestFixture, OneInstructionWithMultipleOperands) {
    CodeInstructionOperand* label_operand = code_instruction_operand_init_label("test");
    CodeInstructionOperand* label_operand2 = code_instruction_operand_init_label("test");
    CodeInstructionOperand* int_operand0 = code_instruction_operand_init_integer(145);
    CodeInstructionOperand* int_operand1 = code_instruction_operand_init_integer(141875);
    CodeInstructionOperand* int_operand2 = code_instruction_operand_init_integer(863);
    CodeInstructionOperand* int_operand3 = code_instruction_operand_init_integer(719);

    EXPECT_TRUE(code_generate_I_JUMP_IF_EQUAL(code_generator, label_operand, int_operand0, int_operand1));
    EXPECT_TRUE(code_generate_I_JUMP_IF_EQUAL(code_generator, label_operand2, int_operand2, int_operand3));

    EXPECT_NE(
            code_generator->first,
            code_generator->last
    );
    EXPECT_EQ(
            code_generator->first->op2,
            int_operand1
    );
    EXPECT_EQ(
            code_generator->last->op2,
            int_operand3
    );
}


TEST_F(CodeGeneratorTestFixture, InvalidOperandType) {
    CodeInstructionOperand* label_operand = code_instruction_operand_init_label("test");
    CodeInstructionOperand* int_operand = code_instruction_operand_init_integer(42);
    DISABLE_LOG(
            EXPECT_FALSE(
                    code_generate_I_WRITE(code_generator, label_operand)
            ) << "Invalid operand type.";
    );
    DISABLE_LOG(
            EXPECT_FALSE(
                    code_generate_I_JUMP_IF_EQUAL(code_generator, label_operand, label_operand, label_operand)
            ) << "Invalid operand type.";
    );
    DISABLE_LOG(
            EXPECT_FALSE(
                    code_generate_I_JUMP_IF_EQUAL(code_generator, label_operand, int_operand, label_operand)
            ) << "Invalid operand type.";
    );
    EXPECT_EQ(
            code_generator->first,
            nullptr
    );
    EXPECT_EQ(
            code_generator->last,
            nullptr
    );

    code_instruction_operand_free(&int_operand);
    code_instruction_operand_free(&label_operand);
}
