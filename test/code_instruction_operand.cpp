#include "gtest/gtest.h"

extern "C" {
#include "../src/code_instruction_operand.h"
#include "../src/code_instruction.h"
}

class CodeInstructionOperandTestFixture : public ::testing::Test {
    protected:
        CodeInstructionOperand* operand = nullptr;

        virtual void TearDown() {
            if(operand != NULL)
                code_instruction_operand_free(&operand);
        }
};

TEST_F(CodeInstructionOperandTestFixture, Label) {
    auto label = const_cast<char*>("label operand");
    operand = code_instruction_operand_init_label(label);

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_LABEL
    );
    EXPECT_STREQ(
            operand->data.label,
            label
    );
}

TEST_F(CodeInstructionOperandTestFixture, Variable) {
    auto symbol = static_cast<SymbolVariable*>(memory_alloc(sizeof(SymbolVariable)));

    operand = code_instruction_operand_init_variable(symbol);

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_VARIABLE
    );
    EXPECT_EQ(
            operand->data.variable,
            symbol
    );
    memory_free(symbol);
}

TEST_F(CodeInstructionOperandTestFixture, String) {
    auto string = string_init();
    string_append_s(string, "static string to generate");
    operand = code_instruction_operand_init_string(string);

    EXPECT_EQ(
            operand->type,
            TYPE_INSTRUCTION_OPERAND_CONSTANT
    );
    EXPECT_STREQ(
            string_content(operand->data.constant.data.string),
            string_content(string)
    );
    EXPECT_EQ(
            operand->data.constant.data_type,
            DATA_TYPE_STRING
    );
    string_free(&string);
}

TEST_F(CodeInstructionOperandTestFixture, StringEscape1) {
    auto string = string_init();
    string_append_s(string, "#foo\\bar\nstr ing");
    char* escaped = code_instruction_operand_escaped_string(string);

    EXPECT_STREQ(
            escaped,
            "\\035foo\\092bar\\010str\\032ing"
    );

    string_free(&string);
    memory_free(escaped);
}

TEST_F(CodeInstructionOperandTestFixture, StringEscape2) {
    auto string = string_init();
    string_append_s(string, "\16ia\17f\12j\40");
    char* escaped = code_instruction_operand_escaped_string(string);

    EXPECT_STREQ(
            escaped,
            "\\014ia\\015f\\010j\\032"
    );

    string_free(&string);
    memory_free(escaped);
}

TEST_F(CodeInstructionOperandTestFixture, StringEscape3
) {
    auto string = string_init();
    string_append_c(string,
                    238);
    char* escaped = code_instruction_operand_escaped_string(string);

    EXPECT_STREQ(
            escaped,
            "\\238"
    );

    string_free(&string);
    memory_free(escaped);
}

TEST_F(CodeInstructionOperandTestFixture, StringRender
) {
    auto string = string_init();
    string_append_s(string,
                    "foobarman\xfe");
    auto operand = code_instruction_operand_init_string(string);
    auto rendered = code_instruction_operand_render(operand);
    EXPECT_STREQ(
            rendered,
            "string@foobarman\\254"
    );

    string_free(&string);
    memory_free(rendered);
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
