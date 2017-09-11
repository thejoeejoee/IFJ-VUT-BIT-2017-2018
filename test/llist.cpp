#include "gtest/gtest.h"

#include "../src/llist.h"
#include "../src/llist.c"

#define NUMBER 42

class LlistTestFixture : public ::testing::Test {

protected:

        LList *llist;

        virtual void SetUp() {
            llist_init(&llist);
        }

        virtual void TearDown() {
            llist_delete(&llist);
        }

};

TEST_F(LlistTestFixture, test_init) {

    /*
     * Test constructor
     */

    // Init seznam
    LList *llist;
    llist_init(&llist);

    // Test for correctly allocated memory
    EXPECT_EQ(
            sizeof(*llist),
            sizeof(LList)
    ) << "Error alocated memory";

    // Test head on null
    EXPECT_EQ(
            NULL,
            llist->head
    ) << "Error head pointer";

    // Test tail on null
    EXPECT_EQ(
            NULL,
            llist->tail
    ) << "Error tail pointer";

    // Clean memory
    free(llist);
}

TEST_F(LlistTestFixture, test_clean) {

    /*
     * Test destructor for list with one item
     */

    // Init list
    LList *llist;
    llist_init(&llist);

    // Add one number
    llist_append(llist, NUMBER);

    // Remove all elements from list
    llist_delete(&llist);

    // Test when list is free
    EXPECT_EQ(
            NULL,
            llist
    ) << "Error destruct list with one item";

}

TEST_F(LlistTestFixture, test_clean_more_items) {

    /*
     * Test destruct for list with more than one item
     */

    // Init list
    LList *llist;
    llist_init(&llist);

    // Add ten elements to the list
    for(int i = 0; i < 2; i++) {
        llist_append(llist, i);
    }

    // clean list
    llist_delete(&llist);

    EXPECT_EQ(
            NULL,
            llist
    ) << "Error destruct list with more than one item";
}

TEST_F(LlistTestFixture, test_add_one_item) {

    // Add one number
    llist_append(llist, NUMBER);

    // Test correct head value
    EXPECT_EQ(
        NUMBER,
        llist->head->value
    ) << "The value in head is incorrect";

    // Test correct tail value
    EXPECT_EQ(
        NUMBER,
        llist->tail->value
    ) << "The value in tail is incorrect";

    // TEST tail and head are the same
    EXPECT_EQ(
            llist->head->value,
            llist->tail->value
    ) << "Values in head and tail are not the same";

    // Test pointers in head
    EXPECT_EQ(
            NULL,
            llist->head->previous
    )<< "Previous pointer in head dont have NULL value";

    EXPECT_EQ(
            NULL,
            llist->head->next
    ) << "Next pinter in head dont have NULL value";

    // Test pointers in tail
    EXPECT_EQ(
            NULL,
            llist->tail->previous
    )<< "Previout pointer in tail dont have NULL value";
    EXPECT_EQ(
            NULL,
            llist->tail->next
    ) << "Next pointer in tail dont have NULL value";

    // remove number from list
    llist_remove_one(llist, NUMBER);
}

TEST_F(LlistTestFixture, test_remove_one) {

    /*
     * Test remove one when llist have just one item
     */

    // Add one number
    llist_append(llist, NUMBER);

    // Remove number
    llist_remove_one(llist, NUMBER);

    EXPECT_EQ(
        NULL,
        llist->head
    ) << "Head is not NULL";

    EXPECT_EQ(
        NULL,
        llist->tail
    ) << "Tail is not NULL";

    // Remove number from list
    llist_remove_one(llist, NUMBER);

}

TEST_F(LlistTestFixture, test_remove_one_more_items) {

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
    LListItem *first = llist->head;
    LListItem *second = first->next;

    // Test values in items
    EXPECT_EQ(
            0,
            first->value
    ) << "Error value in first item";

    EXPECT_EQ(
            2,
            second->value
    ) << "Error value in second item";

    // Test pointers in first item
    EXPECT_EQ(
            NULL,
            first->previous
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
            NULL,
            second->next
    ) << "Error next pointer in second item";

}





