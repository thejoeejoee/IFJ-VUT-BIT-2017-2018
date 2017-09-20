#include "gtest/gtest.h"

extern "C" {
#include "../src/llist.h"
}

class LlistTestFixture : public ::testing::Test {
    protected:
        LList* llist;

        virtual void SetUp() {
            memory_manager_enter(nullptr);
            llist_init(&llist);
        }

        virtual void TearDown() {
            if (llist != nullptr) {
                llist_delete(&llist);
            }
            memory_manager_exit(nullptr);
        }
};

TEST_F(LlistTestFixture, Initialization) {

    /*
     * Test constructor
     */

    // Init seznam
    LList* llist;
    llist_init(&llist);

    // Test for correctly allocated memory
    EXPECT_EQ(
            sizeof(*llist),
            sizeof(LList)
    ) << "Error alocated memory";

    // Test head on null
    EXPECT_EQ(
            llist->head,
            nullptr
    ) << "Error head pointer";

    // Test tail on null
    EXPECT_EQ(
            llist->tail,
            nullptr
    ) << "Error tail pointer";

    // Clean memory
    memory_free(llist);
}

TEST_F(LlistTestFixture, Destruction) {
    /*
     * Test destructor for list with one item
     */

    // Add one number
    llist_append(llist, 42);

    // Remove all elements from list
    llist_delete(&llist);

    // Test when list is free
    EXPECT_EQ(
            llist,
            nullptr
    ) << "Error destruct list with one item";

}

TEST_F(LlistTestFixture, DestructionWithMultipleItems) {

    /*
     * Test destruct for list with more than one item
     */

    // Add ten elements to the list
    for (int i = 0; i < 2; i++) {
        llist_append(llist, i);
    }

    // clean list
    llist_delete(&llist);

    EXPECT_EQ(
            llist,
            nullptr
    ) << "Error destruct list with more than one item";
}

TEST_F(LlistTestFixture, SimpleAppend) {
    constexpr int number = 42;
    // Add one number
    llist_append(llist, number);

    // Test correct head value
    EXPECT_EQ(
            llist->head->value,
            number
    ) << "The value in head is incorrect";

    // Test correct tail value
    EXPECT_EQ(
            llist->tail->value,
            number
    ) << "The value in tail is incorrect";

    // TEST tail and head are the same
    EXPECT_EQ(
            llist->head->value,
            llist->tail->value
    ) << "Values in head and tail are not the same";

    // Test pointers in head
    EXPECT_EQ(
            llist->head->previous,
            nullptr
    ) << "Previous pointer in head doesn't have NULL value";

    EXPECT_EQ(
            llist->head->next,
            nullptr
    ) << "Next pinter in head doesn't have NULL value";

    // Test pointers in tail
    EXPECT_EQ(
            llist->tail->previous,
            nullptr
    ) << "Previout pointer in tail doesn't have NULL value";
    EXPECT_EQ(
            llist->tail->next,
            nullptr
    ) << "Next pointer in tail doesn't have NULL value";

    // remove number from list
    llist_remove_one(llist, number);
}

TEST_F(LlistTestFixture, SimpleRemove) {

    /*
     * Test remove one when llist have just one item
     */
    const int number = 42;
    // Add one number
    llist_append(llist, number);

    // Remove number
    llist_remove_one(llist, number);

    EXPECT_EQ(
            llist->head,
            nullptr
    ) << "Head is not NULL";

    EXPECT_EQ(
            llist->tail,
            nullptr
    ) << "Tail is not NULL";

    // Remove number from list
    llist_remove_one(llist, number);

}

TEST_F(LlistTestFixture, MultipleRemove) {

    /*
     * Test remove one when llist have more than one item
     */

    // Add testing data to the list
    for (int i = 0; i <= 2; i++) {
        llist_append(llist, i);
    }

    // Remove items with value 2 & 4
    llist_remove_one(llist, 1);

    // Get two items from llist
    LListItem* first = llist->head;
    LListItem* second = first->next;

    // Test values in items
    EXPECT_EQ(
            first->value,
            0
    ) << "Error value in first item";

    EXPECT_EQ(
            second->value,
            2
    ) << "Error value in second item";

    // Test pointers in first item
    EXPECT_EQ(
            first->previous,
            nullptr
    ) << "Error previous pointer in first item";

    EXPECT_EQ(
            second,
            first->next
    ) << "Error next pointer in second item";

    // Test pointers in second item
    EXPECT_EQ(
            first,
            second->previous
    ) << "Error previous pointer in second item";

    EXPECT_EQ(
            second->next,
            nullptr
    ) << "Error next pointer in second item";

}





