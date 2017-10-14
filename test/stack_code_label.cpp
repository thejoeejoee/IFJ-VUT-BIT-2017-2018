#include "gtest/gtest.h"

extern "C" {
#include "../src/stack_code_label.h"
}

class StackCodeLabelTestFixture : public ::testing::Test {
    protected:
        Stack* stack;

        virtual void SetUp() {
            stack = stack_code_label_init();
        }

        virtual void TearDown() {
            stack_free(&stack);
        }
};

TEST_F(StackCodeLabelTestFixture, PushAndPop) {
    EXPECT_EQ(
            stack->head,
            nullptr
    ) << "Empty stack after initialization.";

    std::string string = "foobar";
    auto const_string = const_cast<char*>(string.data());
    std::string string2 = "foobar";
    auto const_string2 = const_cast<char*>(string2.data());

    stack_code_label_push(stack, const_string2);
    stack_code_label_push(stack, const_string);

    EXPECT_NE(
            stack->head,
            nullptr
    ) << "Pushed first label to stack.";
    auto item = stack_code_label_pop(stack);
    EXPECT_EQ(
            item->label,
            const_string
    );
    EXPECT_NE(
            stack->head,
            nullptr
    ) << "Pushed first label to stack.";

    auto item2 = stack_code_label_pop(stack);
    EXPECT_EQ(
            item2->label,
            const_string2
    );

    memory_free(item);
    memory_free(item2);

    EXPECT_EQ(
            stack->head,
            nullptr
    ) << "Empty after two pops.";
}

TEST_F(StackCodeLabelTestFixture, HeadValue) {
    std::string string = "header value label";
    auto const_string = const_cast<char*>(string.data());

    stack_code_label_push(stack, const_string);


    EXPECT_EQ(
            strcmp("header value label", stack_code_label_head(stack)),
            0
    ) << "Head value";

    // no item free to test auto free by design
}

TEST_F(StackCodeLabelTestFixture, GetByIndex) {
    EXPECT_EQ(
            stack->head,
            nullptr
    ) << "Empty stack after initialization.";

    std::string string = "foobar bottom";
    auto const_string = const_cast<char*>(string.data());
    std::string string2 = "foobar top";
    auto const_string2 = const_cast<char*>(string2.data());

    stack_code_label_push(stack, const_string2);
    stack_code_label_push(stack, const_string);

    EXPECT_STREQ(
            stack_code_label_get_by_index(stack, 0),
            const_string
    );
    EXPECT_STREQ(
            stack_code_label_get_by_index(stack, 1),
            const_string2
    );
    EXPECT_EQ(
            stack_code_label_get_by_index(stack, 5),
            nullptr
    );
}