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


TEST_F(CodeGeneratorTestFixture, InvalidOperandType) {
    CodeInstructionOperand* label_operand = code_instruction_operand_init_label("test");
    DISABLE_LOG(
            EXPECT_FALSE(
                    code_generate_I_WRITE(code_generator, label_operand)
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
}
