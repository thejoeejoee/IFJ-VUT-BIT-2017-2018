#include "gtest/gtest.h"

extern "C" {
#include "../src/dynamic_string.h"
}

class DynamicStringTestFixture : public ::testing::Test {
    protected:
        String* string;

        virtual void SetUp() {
            string = string_init();
        }

        virtual void TearDown() {
            string_free(string);
        }
};

TEST_F(DynamicStringTestFixture, Append) {
    EXPECT_STREQ(
            string_content(string),
            ""
    ) << "Error creating new dynamic string";

string_append_c(string,
'A');
    EXPECT_STREQ(
            string_content(string),
            "A"
    ) << "Error appending character";

string_append_s(string,
"Bcdef");
    EXPECT_STREQ(
            string_content(string),
"ABcdef"
    ) << "Error appending string";

    auto tmp = string_init();
string_append_s(tmp,
"Ghjkl");
string_append(string, tmp
);

    EXPECT_STREQ(
            string_content(string),
"ABcdefGhjkl"
    ) << "Error appending dynamic string";

    EXPECT_EQ(
            string_length(string),
            11
    ) << "Size of appended string.";

string_free(tmp);
}

TEST_F(DynamicStringTestFixture, Realloc) {

    for(int i = 0; i < 256; i++) {
string_append_c(string,
'A');
    }

    EXPECT_EQ(
            strlen(string_content(string)),
256
    ) << "Error reallocating dynamic string";
}

TEST_F(DynamicStringTestFixture, Clear) {

string_append_s(string,
"Live long, and prosper.");
string_clear(string);

    EXPECT_EQ(
            strlen(string_content(string)),
0
    ) << "Error clearing dynamic string";
}


TEST_F(DynamicStringTestFixture, AppendLong) {
string_clear(string);

    std::string to_append;
    for(int i = 0; i < STRING_INITIAL_CAPACITY * 4; ++i) {
        to_append.append("X");
    }
string_append_s(string, to_append
.

c_str()

);

    EXPECT_EQ(
            string_length(string),
            STRING_INITIAL_CAPACITY * 4
    ) << "Append very long string.";
}
