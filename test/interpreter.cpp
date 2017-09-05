#include "gtest/gtest.h"

#include "../src/interpreter.c"

class SampleTestFixture : public ::testing::Test {
protected:
    int foo;
    int bar;

    SampleTestFixture() {
        foo = 42;
        bar = 5 * foo;
    };
};

TEST_F(SampleTestFixture, multiplied_answer_to_everything) {
    EXPECT_EQ(
            return_multiplied_answer_to_everything(5),
            bar
    );
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}