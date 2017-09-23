#include "gtest/gtest.h"

extern "C" {
#include "../src/dynamic_string.h"
}

class DynamicStringTestFixture : public ::testing::Test {
protected:
	String string;
	virtual void SetUp() {
		string = string_new();
	}
	virtual void TearDown() {
		string_delete(&string);
	}
};

TEST_F(DynamicStringTestFixture, Append) {
	EXPECT_STREQ(
		string_content(&string), ""
	) << "Error creating new dynamic string";
	
	string_append_c(&string, 'A');
	EXPECT_STREQ(
		string_content(&string), "A"
	) << "Error appending character";

	string_append_s(&string, "Bcdef");
	EXPECT_STREQ(
		string_content(&string), "ABcdef"
	) << "Error appending string";

	auto tmp = string_new();
	string_append_s(&tmp, "Ghjkl");
	string_append(&string, &tmp);

	EXPECT_STREQ(
		string_content(&string), "ABcdefGhjkl"
	) << "Error appending dynamic string";

	string_delete(&tmp);
}

TEST_F(DynamicStringTestFixture, Realloc) {

	for (int i = 0; i < 256; i++) {
		string_append_c(&string, 'A');
	}

	EXPECT_EQ(
		strlen(string_content(&string)), 256
	) << "Error reallocating dynamic string";
}
