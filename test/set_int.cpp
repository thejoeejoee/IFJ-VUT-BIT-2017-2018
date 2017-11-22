#include "gtest/gtest.h"

extern "C" {
#include "../src/set_int.h"
}

class SetIntTestFixture: public testing::Test {
    protected:
        SetInt* set;

        void SetUp() override {
            set = set_int_init();
        }

        void TearDown() override {
            set_int_free(&set);
        }
};

TEST_F(SetIntTestFixture, Init) {
    EXPECT_TRUE(
            set_int_is_empty(set)
    ) << "Error new set is not empty.";

    EXPECT_EQ(
            set_int_size(set),
            0
    ) << "Error new set length is not 0.";
}

TEST_F(SetIntTestFixture, AddItems) {
    set_int_add(set, 2);
    set_int_add(set, 5);

    EXPECT_FALSE(
            set_int_is_empty(set)
    ) << "Error set is empty.";

    EXPECT_EQ(
            set_int_size(set),
            2
    ) << "Error set length is not correct.";

    EXPECT_TRUE(
            set_int_contains(set, 2)
    ) << "Error missing value in set.";

    EXPECT_FALSE(
            set_int_contains(set, 6)
    ) << "Error non inserted value in set.";

    EXPECT_TRUE(
            set_int_contains(set, 5)
    ) << "Error missing value in set.";
}

TEST_F(SetIntTestFixture, RemoveItems) {
    set_int_add(set, 3);
    set_int_add(set, 5);
    set_int_add(set, 4);

    set_int_remove(set, 3);

    EXPECT_FALSE(
            set_int_is_empty(set)
    ) << "Error set is empty.";

    EXPECT_EQ(
            set_int_size(set),
            2
    ) << "Error set length is not correct.";

    EXPECT_TRUE(
            set_int_contains(set, 5)
    ) << "Error missing value in set.";

    EXPECT_FALSE(
            set_int_contains(set, 3)
    ) << "Error non inserted value in set.";

    EXPECT_TRUE(
            set_int_contains(set, 4)
    ) << "Error missing value in set.";
}


TEST_F(SetIntTestFixture, ClearItems) {
    set_int_add(set, 3);
    set_int_add(set, 5);
    set_int_add(set, 4);
    set_int_clear(set);

    EXPECT_TRUE(
            set_int_is_empty(set)
    ) << "Error set is not empty.";

    EXPECT_EQ(
            set_int_size(set),
            0
    ) << "Error set length is not correct.";

    EXPECT_FALSE(
            set_int_contains(set, 5)
    ) << "Error non inserted value in set.";

    EXPECT_FALSE(
            set_int_contains(set, 3)
    ) << "Error non inserted value in set.";

    EXPECT_FALSE(
            set_int_contains(set, 4)
    ) << "Error non inserted value in set.";
}
