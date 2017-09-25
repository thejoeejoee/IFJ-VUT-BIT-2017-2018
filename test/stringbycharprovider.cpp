#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"

class StringByCharProviderTestFixture : public ::testing::Test {
    protected:
        StringByCharProvider* provider;

        virtual void SetUp() {
            provider = StringByCharProvider::instance();
        }
};

TEST_F(StringByCharProviderTestFixture, Basic) {
    provider->setString("Foo");

    EXPECT_EQ(
            provider->nextChar(),
            'F'
    ) << "Chars from provider";
    EXPECT_EQ(
            provider->nextChar(),
            'o'
    ) << "Chars from provider";
    EXPECT_EQ(
            provider->nextChar(),
            'o'
    ) << "Chars from provider";
    EXPECT_EQ(
            provider->nextChar(),
            EOF
    ) << "End of stream by EOF";
};


TEST_F(StringByCharProviderTestFixture, Reset) {
    provider->setString("Bar");

    EXPECT_EQ(
            provider->nextChar(),
            'B'
    ) << "Chars from provider";
    EXPECT_EQ(
            provider->nextChar(),
            'a'
    ) << "Chars from provider";
    provider->reset();
    EXPECT_EQ(
            provider->nextChar(),
            EOF
    ) << "End of stream by reset";
};

TEST_F(StringByCharProviderTestFixture, ReplaceString) {
    provider->setString("Bar");

    EXPECT_EQ(
            provider->nextChar(),
            'B'
    ) << "Chars from provider";
    EXPECT_EQ(
            provider->nextChar(),
            'a'
    ) << "Chars from provider";
    provider->setString("X");
    EXPECT_EQ(
            provider->nextChar(),
            'X'
    ) << "Replaced string by another";
    EXPECT_EQ(
            provider->nextChar(),
            EOF
    ) << "Replaced string by another but not next chars";
};

TEST_F(StringByCharProviderTestFixture, ProxyFunction) {
    provider->setString("Bar");

    EXPECT_EQ(
            token_stream(),
            'B'
    ) << "Chars from provider";
    EXPECT_EQ(
            token_stream(),
            'a'
    ) << "Chars from provider";
    provider->setString("X");
    EXPECT_EQ(
            token_stream(),
            'X'
    ) << "Replaced string by another";
    EXPECT_EQ(
            token_stream(),
            EOF
    ) << "Replaced string by another but not next chars";
};