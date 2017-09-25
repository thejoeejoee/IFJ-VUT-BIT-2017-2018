#include "gtest/gtest.h"

extern "C" {
#include "../src/memory.h"
#include "../src/debug.h"
#include "../src/symtable.h"
}

#include "utils/functioncallcounter.h"

void ForeachCount(const char* key, void* data) {}


class HashTableTestFixture : public ::testing::Test {
    protected:
        HashTable* hash_table = nullptr;
        FunctionCallCounter<void, const char*, void*>* callCounter;

        HashTableTestFixture() : testing::Test() {
            callCounter = callCounterInstance(&ForeachCount);
        }

        virtual void SetUp() {
            callCounter->resetCounter();
            hash_table = hash_table_init(2);
        }

        virtual void TearDown() {
            EXPECT_NO_FATAL_FAILURE(hash_table_free(hash_table, FreeData));
        }

        void static FreeData(void* data) {
        }
};

class HashTableWithDataTestFixture : public ::testing::Test {
    protected:
        HashTable* hash_table = nullptr;
        std::vector<const char*> keys = {
                "test1", "test2", "test3", "test4", "test5",
                "test6", "test7", "test8", "test9", "test10"
        };
        FunctionCallCounter<void, const char*, void*>* callCounter;

        HashTableWithDataTestFixture() : testing::Test() {
            callCounter = callCounterInstance(&ForeachCount);
        }

        virtual void SetUp() {
            callCounter->resetCounter();
            hash_table = hash_table_init(2);

            // Insert items
            for(auto &key : keys) {
                hash_table_get_or_create(hash_table, key);
            }
        }

        virtual void TearDown() {
            EXPECT_NO_FATAL_FAILURE(hash_table_free(hash_table, FreeData));
        }

        void static FreeData(void* data) {
        }
};

TEST_F(HashTableTestFixture, Initialization) {
    EXPECT_NE(hash_table, nullptr) << "Initialized hash table is not null";
}

TEST_F(HashTableTestFixture, BucketCount) {
    EXPECT_EQ(hash_table_bucket_count(hash_table), 2) << "Initialized hash table with 2 buckets.";
}

TEST_F(HashTableTestFixture, SizeEmpty) {
    EXPECT_EQ(
            hash_table_size(hash_table),
            0
    ) << "Size is not 0";
}

TEST_F(HashTableTestFixture, InsertItems) {
    std::vector<const char*> keys = {"test1", "test2", "test3"};

    // Test function
    DISABLE_LOG({
                    ASSERT_EQ(
                            hash_table_get_or_create(nullptr, "null"),
                            nullptr
                    ) << "No item should be created with nullptr passed as table";
                });

    // Insert items
    for(auto &key : keys) {
        EXPECT_NE(
                hash_table_get_or_create(hash_table, key),
                nullptr
        ) << "Function should return item ptr";
    }

    // Check size
    EXPECT_EQ(
            hash_table_size(hash_table),
            keys.size()
    ) << "Hash table should have 3 items";
}

TEST_F(HashTableTestFixture, MemoryDeallocation) {
    // TODO: Test memory deallocation
}

TEST_F(HashTableTestFixture, GetOnEmptyTable) {
    DISABLE_LOG(
            ASSERT_EQ(
                    hash_table_get(nullptr, "nokey"),
                    nullptr
            ) << "nullptr as hash table should return nullptr";
    );

    EXPECT_EQ(
            hash_table_get(hash_table, "nokey"),
            nullptr
    ) << "Empty hash table should return nullptr";
}

TEST_F(HashTableWithDataTestFixture, GetInvalidItem) {
    HashTableListItem* item = hash_table_get(hash_table, "invalid");

    ASSERT_EQ(
            item,
            nullptr
    ) << "Found item should be nullptr";
}

TEST_F(HashTableWithDataTestFixture, GetValidItem) {

    HashTableListItem* item = hash_table_get(hash_table, keys[1]);

    ASSERT_NE(
            item,
            nullptr
    ) << "Found item should not be nullptr";

    EXPECT_STREQ(
            item->key,
            keys[1]
    ) << "The items key should be equal to searched key";
}

TEST_F(HashTableWithDataTestFixture, GetOrCreateWithData) {
    EXPECT_NE(
            hash_table_get_or_create(hash_table, keys.at(keys.size() / 2)),
            nullptr
    ) << "Valid search on hash table with data.";
}

TEST_F(HashTableWithDataTestFixture, DeleteInvalidItem) {
    EXPECT_FALSE(hash_table_remove(hash_table, "invalid", FreeData)) << "Invalid key should return false";
}

TEST_F(HashTableWithDataTestFixture, DeleteValidItem) {
    EXPECT_TRUE(hash_table_remove(hash_table, keys.at(keys.size() / 2), FreeData))
                        << "Deleting valid key should return true";
}

TEST_F(HashTableTestFixture, DeleteOnEmptyTable) {
    ASSERT_FALSE(hash_table_remove(hash_table, "nokey", FreeData)) << "Empty table should return false";
}


TEST_F(HashTableTestFixture, InvalidDelete) {
    DISABLE_LOG({
                    ASSERT_FALSE(hash_table_remove(nullptr, "nokey", FreeData)) << "Null table should return false";
                    ASSERT_FALSE(
                            hash_table_remove(hash_table, nullptr, FreeData)) << "Null key should return false";
                });
}

TEST_F(HashTableTestFixture, MoveEmptyTable) {
    HashTable* table = nullptr;
    EXPECT_NE(
            (table = hash_table_move(5, hash_table)),
            nullptr
    ) << "New table should not be nullptr";

    hash_table_free(table, FreeData);
}

TEST_F(HashTableWithDataTestFixture, MoveTableWithItems) {
    size_t items = hash_table_size(hash_table);
    HashTable* new_table = hash_table_move(keys.size() * 2, hash_table);

    ASSERT_NE(
            new_table,
            nullptr
    ) << "New table should not be nullptr";

    EXPECT_EQ(
            hash_table_size(new_table),
            items
    ) << "New hash table should have the same size";

    EXPECT_EQ(
            hash_table_size(hash_table),
            0
    ) << "Source table should have no items.";

    for(auto key : keys) {
        EXPECT_NE(
                hash_table_get(new_table, key),
                nullptr
        ) << "All items were copied.";
    }

    hash_table_free(new_table, FreeData);
}

TEST_F(HashTableTestFixture, MoveTableInvalid) {
    DISABLE_LOG({
                    EXPECT_EQ(
                            hash_table_move(5, nullptr),
                            nullptr
                    ) << "Null table should return nullptr";
                });
}

TEST_F(HashTableTestFixture, Foreach) {
    DISABLE_LOG({

                    hash_table_foreach(nullptr, callCounter->wrapper());
                });

    EXPECT_EQ(
            callCounter->callCount(),
            0
    ) << "Callback function should not be called";

    callCounter->resetCounter();

    hash_table_foreach(hash_table, callCounter->wrapper());

    EXPECT_EQ(
            callCounter->callCount(),
            0
    ) << "Callback function should not be called";
}

TEST_F(HashTableWithDataTestFixture, Foreach) {
    hash_table_foreach(hash_table, callCounter->wrapper());

    EXPECT_EQ(
            callCounter->callCount(),
            keys.size()
    ) << "Callback function should be called 5 times";
}
