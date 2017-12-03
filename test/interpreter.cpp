#include "gtest/gtest.h"

extern "C" {
#include "../src/code_instruction_operand.h"
#include "../src/interpreter.h"
#include "../src/code_generator.h"
#include "../src/code_constructor.h"
#include "../src/symtable.h"
}

class InterpreterTestFixture : public ::testing::Test {
    protected:
        CodeConstructor* constructor;
        Interpreter* interpreter;
        SymbolVariable* buffer;

        virtual void SetUp() {
            buffer = symbol_variable_init("tmp1");
            constructor = code_constructor_init();
            interpreter = interpreter_init(buffer);
        }

        virtual void TearDown() {
            code_constructor_free(&constructor);
            interpreter_free(&interpreter);
            symbol_variable_single_free(&buffer);
        }
};

TEST_F(InterpreterTestFixture, IntegerWithoutBuffer) {
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(-5)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(-4)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(2)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(42)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(-84)
    );
    GENERATE_CODE(I_ADD_STACK);
    GENERATE_CODE(I_SUB_STACK);
    GENERATE_CODE(I_DIV_STACK);
    GENERATE_CODE(I_MUL_STACK);

    CodeInstructionOperand* operand = interpreter_evaluate_instruction_block(
            interpreter,
            constructor->generator->first,
            constructor->generator->last
    );
    ASSERT_NE(
            operand,
            nullptr
    );

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_INTEGER
    );
    EXPECT_EQ(
            operand->data.constant.data.integer,
            -55
    );
    code_instruction_operand_free(&operand);
}

TEST_F(InterpreterTestFixture, DoubleWithoutBuffer) {
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(12.1)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(10.2)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(3.5)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(74.8)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(10.12)
    );
    GENERATE_CODE(I_ADD_STACK);
    GENERATE_CODE(I_SUB_STACK);
    GENERATE_CODE(I_DIV_STACK);
    GENERATE_CODE(I_MUL_STACK);

    CodeInstructionOperand* operand = interpreter_evaluate_instruction_block(
            interpreter,
            constructor->generator->first,
            constructor->generator->last
    );
    ASSERT_NE(
            operand,
            nullptr
    );

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_DOUBLE
    );
    EXPECT_EQ(
            operand->data.constant.data.double_,
            (((10.12 + 74.8) - 3.5) / 10.2) * 12.1
    );
    code_instruction_operand_free(&operand);
}

TEST_F(InterpreterTestFixture, ConversionDoubleToInt) {
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(12.5)
    );
    GENERATE_CODE(I_FLOAT_ROUND_TO_EVEN_INT_STACK);
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(13.5)
    );
    GENERATE_CODE(I_FLOAT_ROUND_TO_EVEN_INT_STACK);
    GENERATE_CODE(I_ADD_STACK);

    CodeInstructionOperand* operand = interpreter_evaluate_instruction_block(
            interpreter,
            constructor->generator->first,
            constructor->generator->last
    );
    ASSERT_NE(
            operand,
            nullptr
    );

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_INTEGER
    );
    EXPECT_EQ(
            operand->data.constant.data.integer,
            14 + 12
    );
    code_instruction_operand_free(&operand);
}

TEST_F(InterpreterTestFixture, ConversionIntToDouble) {
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(12)
    );
    GENERATE_CODE(I_INT_TO_FLOAT_STACK);
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(13)
    );
    GENERATE_CODE(I_INT_TO_FLOAT_STACK);
    GENERATE_CODE(I_ADD_STACK);

    CodeInstructionOperand* operand = interpreter_evaluate_instruction_block(
            interpreter,
            constructor->generator->first,
            constructor->generator->last
    );
    ASSERT_NE(
            operand,
            nullptr
    );

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_DOUBLE
    );
    EXPECT_EQ(
            operand->data.constant.data.double_,
            12. + 13.
    );
    code_instruction_operand_free(&operand);
}

TEST_F(InterpreterTestFixture, ConversionWithBuffer) {
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(12.5)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(10)
    );
    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(buffer)
    );
    GENERATE_CODE(I_FLOAT_ROUND_TO_EVEN_INT_STACK);
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_variable(buffer)
    );
    GENERATE_CODE(I_ADD_STACK);

    CodeInstructionOperand* operand = interpreter_evaluate_instruction_block(
            interpreter,
            constructor->generator->first,
            constructor->generator->last
    );
    ASSERT_NE(
            operand,
            nullptr
    );

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_INTEGER
    );
    EXPECT_EQ(
            operand->data.constant.data.integer,
            22
    );
    code_instruction_operand_free(&operand);
}

TEST_F(InterpreterTestFixture, Booleans) {
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(true)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(false)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(true)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(true)
    );

    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_boolean(false)
    );
    GENERATE_CODE(I_NOT_STACK);
    GENERATE_CODE(I_AND_STACK);
    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(buffer)
    );
    GENERATE_CODE(I_NOT_STACK);
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_variable(buffer)
    );
    GENERATE_CODE(I_AND_STACK);
    GENERATE_CODE(I_NOT_STACK);
    GENERATE_CODE(I_OR_STACK);
    GENERATE_CODE(I_OR_STACK);

    CodeInstructionOperand* operand = interpreter_evaluate_instruction_block(
            interpreter,
            constructor->generator->first,
            constructor->generator->last
    );
    ASSERT_NE(
            operand,
            nullptr
    );

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_BOOLEAN
    );
    EXPECT_EQ(
            operand->data.constant.data.boolean,
            true
    );
    code_instruction_operand_free(&operand);
}

TEST_F(InterpreterTestFixture, Comparing) {
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(12.14)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(25)
    );
    GENERATE_CODE(I_GREATER_THEN_STACK);
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(120.14)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(-12.14)
    );
    GENERATE_CODE(I_LESSER_THEN_STACK);
    GENERATE_CODE(I_AND_STACK);

    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(120.14)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(120.14)
    );
    GENERATE_CODE(I_EQUAL_STACK);
    GENERATE_CODE(I_AND_STACK);

    CodeInstructionOperand* operand = interpreter_evaluate_instruction_block(
            interpreter,
            constructor->generator->first,
            constructor->generator->last
    );
    ASSERT_NE(
            operand,
            nullptr
    );

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_BOOLEAN
    );
    EXPECT_EQ(
            operand->data.constant.data.boolean,
            true
    );
    code_instruction_operand_free(&operand);
}

TEST_F(InterpreterTestFixture, Division) {
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(12)
    );
    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(buffer)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(48)
    );
    GENERATE_CODE(I_INT_TO_FLOAT_STACK);
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_variable(buffer)
    );
    GENERATE_CODE(I_DIV_STACK);

    CodeInstructionOperand* operand = interpreter_evaluate_instruction_block(
            interpreter,
            constructor->generator->first,
            constructor->generator->last
    );
    ASSERT_NE(
            operand,
            nullptr
    );

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_DOUBLE
    );
    EXPECT_EQ(
            operand->data.constant.data.double_,
            4.0
    );
    code_instruction_operand_free(&operand);
}

TEST_F(InterpreterTestFixture, Subs) {
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_double(12)
    );
    GENERATE_CODE(
            I_POP_STACK,
            code_instruction_operand_init_variable(buffer)
    );
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_integer(48)
    );
    GENERATE_CODE(I_INT_TO_FLOAT_STACK);
    GENERATE_CODE(
            I_PUSH_STACK,
            code_instruction_operand_init_variable(buffer)
    );
    GENERATE_CODE(I_SUB_STACK);

    CodeInstructionOperand* operand = interpreter_evaluate_instruction_block(
            interpreter,
            constructor->generator->first,
            constructor->generator->last
    );
    ASSERT_NE(
            operand,
            nullptr
    );

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_DOUBLE
    );
    EXPECT_EQ(
            operand->data.constant.data.double_,
            36.0
    );
    code_instruction_operand_free(&operand);
}
