#include "gtest/gtest.h"

#include "../src/memory.c"

class MemoryManagerTestFixture : public ::testing::Test {
    protected:
        MemoryManager memory_manager;

        virtual void SetUp() {
            memory_manager_enter(&memory_manager);
        }

        virtual void TearDown() {
            memory_manager_exit(&memory_manager);
        }

};

TEST_F(MemoryManagerTestFixture, Initialization) {
    EXPECT_EQ(memory_manager.head, nullptr) << "Empty head after initialization.";
}

TEST_F(MemoryManagerTestFixture, Destruction) {
    memory_manager_exit(&memory_manager);
    EXPECT_EQ(memory_manager.head, nullptr) << "Empty head after destruction.";
}

TEST_F(MemoryManagerTestFixture, InvalidMemoryAllocation) {
    EXPECT_EQ(memory_manager_malloc(0, nullptr, 0, nullptr, &memory_manager), nullptr) << "Invalid size.";
}

TEST_F(MemoryManagerTestFixture, InvalidMemoryDealocation) {
    // TODO: free does return void.. so no method to check valid address
    memory_manager_free(&memory_manager, &memory_manager);
}

TEST_F(MemoryManagerTestFixture, SingleMemoryAllocation) {
    void* memory = memory_manager_malloc(8, "bar", 1, "foo", &memory_manager);

    MemoryManagerPage* page = memory_manager.head;
    ASSERT_NE(page, nullptr) << "Allocated page.";

    EXPECT_EQ(page->size, 8) << "Saved page size.";
    EXPECT_EQ(page->next, nullptr) << "No next page.";
    EXPECT_EQ(page->address, memory) << "Saved allocated memory address.";
    EXPECT_TRUE(page->allocated) << "Marked as allocated.";
    EXPECT_STREQ(page->info, "bar:1:foo()") << "Saved info about allocation.";

    memory_manager_free(memory, &memory_manager);

    EXPECT_EQ(page->size, 8) << "Saved page size after free.";
    EXPECT_EQ(page->next, nullptr) << "No next page after free.";
    EXPECT_EQ(page->address, nullptr) << "Freed memory after memory manager free.";
    EXPECT_FALSE(page->allocated) << "Marked as freed.";
    EXPECT_STREQ(page->info, "bar:1:foo()") << "Saved info about allocation after free.";
}

TEST_F(MemoryManagerTestFixture, MultipleMemoryAllocation) {
    void* first_memory = memory_manager_malloc(8, "bar", 1, "foo", &memory_manager);
    void* second_memory = memory_manager_malloc(16, "file", 1, "function", &memory_manager);

    MemoryManagerPage* second_page = memory_manager.head;
    ASSERT_NE(second_page, nullptr) << "Allocated second_page.";

    EXPECT_EQ(second_page->size, 16) << "Saved page size.";
    EXPECT_NE(second_page->next, nullptr) << "Has next page.";
    EXPECT_EQ(second_page->address, second_memory) << "Saved allocated of second memory address.";
    EXPECT_TRUE(second_page->allocated) << "Marked as allocated.";
    EXPECT_STREQ(second_page->info, "file:1:function()") << "Saved info about allocation.";

    MemoryManagerPage* first_page = second_page->next;

    EXPECT_EQ(first_page->size, 8) << "Saved page size after free.";
    EXPECT_EQ(first_page->next, nullptr) << "No next page after free.";
    EXPECT_EQ(first_page->address, first_memory) << "Saved memory pointer.";
    EXPECT_TRUE(first_page->allocated) << "Marked as allocated.";
    EXPECT_STREQ(first_page->info, "bar:1:foo()") << "Saved info about allocation.";

    memory_manager_free(first_memory, &memory_manager);

    EXPECT_FALSE(first_page->allocated) << "Allocation flag for freed memory.";
    EXPECT_EQ(first_page->address, nullptr) << "Nulled address of freed page.";
}


