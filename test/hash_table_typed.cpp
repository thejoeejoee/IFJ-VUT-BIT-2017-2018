#include "gtest/gtest.h"

extern "C" {
#include "../src/memory.h"
#include "../src/debug.h"
#include "../src/symtable.h"
}

#include "utils/functioncallcounter.h"

typedef struct test_structure_t {
    bool foo;
} TestStructure;

HASH_TABLE_TYPED_HEADERS(TestStructure, test_structure);

HASH_TABLE_TYPED_IMPLEMENTATION(TestStructure, test_structure);

class HashTableTypedTestFixture : public ::testing::Test {
    protected:
        HashTableTestStructure* hash_table = nullptr;

        virtual void SetUp() {
            hash_table = hash_table_test_structure_init(2, FreeData);
        }

        virtual void TearDown() {
            hash_table_test_structure_free(hash_table);
        }

        void static FreeData(TestStructure* data) {
        }
};

TEST_F(HashTableTypedTestFixture, Finding) {
    EXPECT_EQ(
            hash_table_test_structure_get(hash_table, "unknown"),
            nullptr
    ) << "Get on empty typed table.";

    HashTableListItemTestStructure* item = hash_table_test_structure_get_or_create(hash_table, "unknown");
    HashTableListItemTestStructure* found_item;

    EXPECT_NE(
            item,
            nullptr
    ) << "Get on empty typed table.";
    TestStructure* data;
    data = item->data = (TestStructure*) memory_alloc(sizeof(TestStructure));
    item->data->foo = true;

    found_item = hash_table_test_structure_get(hash_table, "unknown");
    EXPECT_NE(
            found_item,
            nullptr
    ) << "Successfully found item.";

    found_item->data->foo = false;

    EXPECT_EQ(
            found_item->data->foo,
            item->data->foo
    ) << "Same data on found and previous created item";

    memory_free(data);
}
