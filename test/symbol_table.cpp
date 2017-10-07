#include "gtest/gtest.h"

extern "C" {
#include "../src/memory.h"
#include "../src/debug.h"
#include "../src/symtable.h"
}

#include "utils/functioncallcounter.h"

void ForeachCount(const char* key, void* data) {}


class SymbolTableTestFixture : public ::testing::Test {
    protected:
        SymbolTable* symbol_table = nullptr;
        FunctionCallCounter<void, const char*, void*>* callCounter;

        SymbolTableTestFixture() : testing::Test() {
            callCounter = callCounterInstance(&ForeachCount);
        }

        virtual void SetUp() {
            callCounter->resetCounter();
            symbol_table = symbol_table_init(2, FreeData);
        }

        virtual void TearDown() {
            symbol_table_free(symbol_table);
        }

        void static FreeData(void* data) {
        }
};

class SymbolTableWithDataTestFixture : public ::testing::Test {
    protected:
        SymbolTable* symbol_table = nullptr;
        std::vector<const char*> keys = {
                "test1", "test2", "test3", "test4", "test5",
                "test6", "test7", "test8", "test9", "test10"
        };
        FunctionCallCounter<void, const char*, void*>* callCounter;

        SymbolTableWithDataTestFixture() : testing::Test() {
            callCounter = callCounterInstance(&ForeachCount);
        }

        virtual void SetUp() {
            callCounter->resetCounter();
            symbol_table = symbol_table_init(2, FreeData);

            // Insert items
            for(auto &key : keys) {
                symbol_table_get_or_create(symbol_table, key);
            }
        }

        virtual void TearDown() {
            symbol_table_free(symbol_table);
        }

        void static FreeData(void* data) {
        }
};

TEST_F(SymbolTableTestFixture, Initialization) {
    EXPECT_NE(symbol_table, nullptr) << "Initialized hash table is not null";
}

TEST_F(SymbolTableTestFixture, BucketCount) {
    EXPECT_EQ(symbol_table_bucket_count(symbol_table), 2) << "Initialized hash table with 2 buckets.";
}

TEST_F(SymbolTableTestFixture, SizeEmpty) {
    EXPECT_EQ(
            symbol_table_size(symbol_table),
            0
    ) << "Size is not 0";
}

TEST_F(SymbolTableTestFixture, InsertItems) {
    std::vector<const char*> keys = {"test1", "test2", "test3"};

    // Test function
    DISABLE_LOG({
                    ASSERT_EQ(
                            symbol_table_get_or_create(nullptr, "null"),
                            nullptr
                    ) << "No item should be created with nullptr passed as table";
                });

    // Insert items
    for(auto &key : keys) {
        EXPECT_NE(
                symbol_table_get_or_create(symbol_table, key),
                nullptr
        ) << "Function should return item ptr";
    }

    // Check size
    EXPECT_EQ(
            symbol_table_size(symbol_table),
            keys.size()
    ) << "Hash table should have 3 items";
}

TEST_F(SymbolTableTestFixture, MemoryDeallocation) {
    // TODO: Test memory deallocation
}

TEST_F(SymbolTableTestFixture, GetOnEmptyTable) {
    DISABLE_LOG(
            ASSERT_EQ(
                    symbol_table_get(nullptr, "nokey"),
                    nullptr
            ) << "nullptr as hash table should return nullptr";
    );

    EXPECT_EQ(
            symbol_table_get(symbol_table, "nokey"),
            nullptr
    ) << "Empty hash table should return nullptr";
}

TEST_F(SymbolTableWithDataTestFixture, GetInvalidItem) {
    SymbolTableListItem* item = symbol_table_get(symbol_table, "invalid");

    ASSERT_EQ(
            item,
            nullptr
    ) << "Found item should be nullptr";
}

TEST_F(SymbolTableWithDataTestFixture, GetValidItem) {

    SymbolTableListItem* item = symbol_table_get(symbol_table, keys[1]);

    ASSERT_NE(
            item,
            nullptr
    ) << "Found item should not be nullptr";

    EXPECT_STREQ(
            item->key,
            keys[1]
    ) << "The items key should be equal to searched key";
}

TEST_F(SymbolTableWithDataTestFixture, GetOrCreateWithData) {
    EXPECT_NE(
            symbol_table_get_or_create(symbol_table, keys.at(keys.size() / 2)),
            nullptr
    ) << "Valid search on hash table with data.";
}

TEST_F(SymbolTableWithDataTestFixture, DeleteInvalidItem) {
    EXPECT_FALSE(symbol_table_remove(symbol_table, "invalid")) << "Invalid key should return false";
}

TEST_F(SymbolTableWithDataTestFixture, DeleteValidItem) {
    EXPECT_TRUE(
            symbol_table_remove(symbol_table, keys.at(keys.size() / 2))
    ) << "Deleting valid key should return true";
    EXPECT_TRUE(
            symbol_table_remove(symbol_table, keys.at(0))
    ) << "Remove first item.";
}

TEST_F(SymbolTableTestFixture, DeleteOnEmptyTable) {
    ASSERT_FALSE(symbol_table_remove(symbol_table, "nokey")) << "Empty table should return false";
}


TEST_F(SymbolTableTestFixture, InvalidDelete) {
    DISABLE_LOG({
                    ASSERT_FALSE(symbol_table_remove(nullptr, "nokey")) << "Null table should return false";
                    ASSERT_FALSE(
                            symbol_table_remove(symbol_table, nullptr)) << "Null key should return false";
                });
}

TEST_F(SymbolTableTestFixture, MoveEmptyTable) {
    SymbolTable* table = nullptr;
    EXPECT_NE(
            (table = symbol_table_move(5, symbol_table)),
            nullptr
    ) << "New table should not be nullptr";

    symbol_table_free(table);
}

TEST_F(SymbolTableWithDataTestFixture, MoveTableWithItems) {
    size_t items = symbol_table_size(symbol_table);
    SymbolTable* new_table = symbol_table_move(keys.size() * 2, symbol_table);

    ASSERT_NE(
            new_table,
            nullptr
    ) << "New table should not be nullptr";

    EXPECT_EQ(
            symbol_table_size(new_table),
            items
    ) << "New hash table should have the same size";

    EXPECT_EQ(
            symbol_table_size(symbol_table),
            0
    ) << "Source table should have no items.";

    for(auto key : keys) {
        EXPECT_NE(
                symbol_table_get(new_table, key),
                nullptr
        ) << "All items were copied.";
    }

    symbol_table_free(new_table);
}

TEST_F(SymbolTableTestFixture, MoveTableInvalid) {
    DISABLE_LOG({
                    EXPECT_EQ(
                            symbol_table_move(5, nullptr),
                            nullptr
                    ) << "Null table should return nullptr";
                });
}

TEST_F(SymbolTableTestFixture, Foreach) {
    DISABLE_LOG({

                    symbol_table_foreach(nullptr, callCounter->wrapper());
                });

    EXPECT_EQ(
            callCounter->callCount(),
            0
    ) << "Callback function should not be called";

    callCounter->resetCounter();

    symbol_table_foreach(symbol_table, callCounter->wrapper());

    EXPECT_EQ(
            callCounter->callCount(),
            0
    ) << "Callback function should not be called";
}

TEST_F(SymbolTableWithDataTestFixture, Foreach) {
    symbol_table_foreach(symbol_table, callCounter->wrapper());

    EXPECT_EQ(
            callCounter->callCount(),
            keys.size()
    ) << "Callback function should be called 5 times";
}
