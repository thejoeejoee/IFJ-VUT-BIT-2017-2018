#include "gtest/gtest.h"

#include "../src/ial.h"
#include "../src/ial.c"

class HashTableTestFixture : public ::testing::Test {
    protected:
        HashTable* hash_table;

        virtual void SetUp() {
            hash_table = hash_table_init(8);
        }

        virtual void TearDown() {
            hash_table_free(hash_table, FreeData);
        }

        void static FreeData(void* data) {
            free(data);
        }

};

TEST_F(HashTableTestFixture, Initialization) {
    EXPECT_TRUE(hash_table != NULL);
}


