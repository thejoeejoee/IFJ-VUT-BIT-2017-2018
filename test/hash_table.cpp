#include "gtest/gtest.h"

#include "../src/ial.c"

class HashTableTestFixture : public ::testing::Test {
    protected:
        HashTable* hash_table = nullptr;
        int foreach_count = 0;

        virtual void SetUp() {
            memory_manager_enter(NULL);
            hash_table = hash_table_init(8);
        }

        virtual void TearDown() {
            EXPECT_NO_FATAL_FAILURE(hash_table_free(hash_table, FreeData));
            memory_manager_exit(NULL);
        }

        void static FreeData(void* data) {
            if (data != nullptr)
                free(data);
        }

        void ForeachCount(const char* key, void* data) {
            foreach_count++;
        }

};

class HashTableWithDataTestFixture : public ::testing::Test {
    protected:
        HashTable* hash_table = nullptr;
        static constexpr size_t n_samples = 5;
        const char* keys[n_samples] = {"test1", "test2", "test3", "test4", "test5"};
        static int foreach_count;

        virtual void SetUp() {
            memory_manager_enter(NULL);
            hash_table = hash_table_init(n_samples);

            // Insert items
            for (auto &key : keys) {
                hash_table_get_or_create(hash_table, key);
            }
        }

        virtual void TearDown() {
            EXPECT_NO_FATAL_FAILURE(hash_table_free(hash_table, FreeData));
            memory_manager_exit(NULL);
        }

        void static FreeData(void* data) {
            if (data != nullptr)
                free(data);
        }

        void static ForeachCount(const char* key, void* data) {
            foreach_count++;
        }
};

TEST_F(HashTableTestFixture, Initialization) {
    EXPECT_NE(hash_table, nullptr) << "Initialized hash table is not null";
}

TEST_F(HashTableTestFixture, SizeEmpty) {
    EXPECT_EQ(
            hash_table_size(hash_table),
            0
    ) << "Size is not 0";
}

TEST_F(HashTableTestFixture, InsertItems) {
    constexpr size_t n_samples = 3;
    const char* keys[n_samples] = {"test1", "test2", "test3"};

    // Test function
    DISABLE_LOG({
                    ASSERT_EQ(
                            hash_table_get_or_create(nullptr, "null"),
                            nullptr
                    ) << "No item should be created with nullptr passed as table";
                });

    // Insert items
    for (int i = 0; i < n_samples; i++) {
        EXPECT_NE(
                hash_table_get_or_create(hash_table, keys[i]),
                nullptr
        ) << "Function should return item ptr";
    }

    // Check size
    EXPECT_EQ(
            hash_table_size(hash_table),
            n_samples
    ) << "Hash table should have " << n_samples << " items";
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

TEST_F(HashTableWithDataTestFixture, DeleteInvalidItem) {
    EXPECT_FALSE(hash_table_delete(hash_table, "invalid", FreeData)) << "Invalid key should return false";
}

TEST_F(HashTableWithDataTestFixture, DeleteValidItem) {
    EXPECT_TRUE(hash_table_delete(hash_table, keys[2], FreeData)) << "Deleting valid key should return true";
}

TEST_F(HashTableTestFixture, DeleteOnEmptyTable) {
    ASSERT_FALSE(hash_table_delete(hash_table, "nokey", FreeData)) << "Empty table should return false";
}


TEST_F(HashTableTestFixture, InvalidDelete) {
    DISABLE_LOG({
                    ASSERT_FALSE(hash_table_delete(nullptr, "nokey", FreeData)) << "Null table should return false";
                    ASSERT_FALSE(
                            hash_table_delete(hash_table, nullptr, FreeData)) << "Null key should return false";
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

TEST_F(HashTableWithDataTestFixture, DISABLED_MoveTableWithItems) {
    HashTable* new_table = hash_table_move(n_samples + 5, hash_table);

    ASSERT_NE(
            new_table,
            nullptr
    ) << "New table should not be nullptr";

    EXPECT_EQ(
            hash_table_size(new_table),
            hash_table_size(hash_table)
    ) << "New hash table should have the same size";

    hash_table_free(new_table, FreeData);

    // TODO: check all moved items
}

TEST_F(HashTableTestFixture, MoveTableInvalid) {
    DISABLE_LOG({
                    EXPECT_EQ(
                            hash_table_move(5, nullptr),
                            nullptr
                    ) << "Null table should return nullptr";
                });
}

// TODO: Fix foreach tests
TEST_F(HashTableTestFixture, ForeachInvalid) {
    /*foreach_count = 0;

    hash_table_foreach(nullptr, ForeachCount);

    EXPECT_EQ(
            foreach_count,
            0
    ) << "Callback function should not be called";*/
}

TEST_F(HashTableTestFixture, ForeachOnEmptyTable) {
    /*HashTableTestFixture::foreach_count = 0;

    hash_table_foreach(hash_table, ForeachCount);

    EXPECT_EQ(
            HashTableTestFixture::foreach_count,
            0
    ) << "Callback function should not be called";*/
}

TEST_F(HashTableWithDataTestFixture, Foreach) {
    /*foreach_count = 0;

    hash_table_foreach(hash_table, ForeachCount);

    EXPECT_EQ(
        foreach_count,
        n_samples
    ) << "Callback function should be called " << n_samples << " times";*/
}