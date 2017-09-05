#include "gtest/gtest.h"

#include "../src/interpreter.c"

TEST(Interpreter, multiplied_answer_to_everything) {
    EXPECT_EQ(
            return_multiplied_answer_to_everything(5),
            5 * 42
    );
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}