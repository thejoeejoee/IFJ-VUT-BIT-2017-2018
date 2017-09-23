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

    for(int i = 0; i < 10; i++) {
        EXPECT_TRUE(
                char_stack_push(stack, 'a')
        ) << "Error pushing item";
    }

    for(int i = 0; i < 10; i++) {
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
