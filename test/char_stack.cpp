#include "gtest/gtest.h"

extern "C" {
#include "../src/char_stack.h"
}

class CharStackTestFixture : public ::testing::Test {
    protected:
        CharStack* stack;

        virtual void SetUp() {
            stack = char_stack_init();
        }

        virtual void TearDown() {
            char_stack_free(&stack);
        }
};

TEST_F(CharStackTestFixture, AddItem) {
    EXPECT_TRUE(
            char_stack_push(stack, 'a')
    ) << "Error pushing item";


    EXPECT_EQ(
            char_stack_pop(stack),
            'a'

    ) << "Error error popping item";

    EXPECT_EQ(
            char_stack_pop(stack),
            EOF

    ) << "Error error popping item";

    for(int i = 0; i < CHAR_STACK_MAX_LENGTH; i++) {
        EXPECT_TRUE(
                char_stack_push(stack, 'a')
        ) << "Error pushing item";
    }

    for(int i = 0; i < CHAR_STACK_MAX_LENGTH; i++) {
        EXPECT_EQ(
                char_stack_pop(stack),
                'a'
        ) << "Error error popping item";
    }

    EXPECT_EQ(
            char_stack_pop(stack),
            EOF
    ) << "Error error popping item";
}

TEST_F(CharStackTestFixture, ClearStack) {
    char_stack_empty(stack);
    EXPECT_EQ(
            char_stack_pop(stack),
            EOF
    ) << "Error clearing char stack";

    for(int i = 0; i < CHAR_STACK_MAX_LENGTH; i++) {
        char_stack_push(stack, 'a');
    }
    char_stack_empty(stack);
    EXPECT_EQ(
            char_stack_pop(stack),
            EOF
    ) << "Error clearing char stack";
}
