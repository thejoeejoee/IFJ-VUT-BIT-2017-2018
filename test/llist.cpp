#include "gtest/gtest.h"

#include "../src/llist.h"
#include "../src/llist.c"

#define NUMBER 42

class LlistTestFixture : public ::testing::Test {
protected:


};

TEST_F(LlistTestFixture, test_init) {

    // Init seznam
    LList *llist;
    llist_init(&llist);

    // Test for correctly allocated memory
    EXPECT_EQ(
            sizeof(*llist),
            sizeof(LList)
    );

    // Test head on null
    EXPECT_EQ(
            NULL,
            llist->head
    );

    // Test tail on null
    EXPECT_EQ(
            NULL,
            llist->tail
    );

    // Clean memory
    free(llist);
}

TEST_F(LlistTestFixture, test_add_one_point) {

    // Init seznam
    LList *llist;
    llist_init(&llist);

    // Add one number
    llist_append(llist, NUMBER);

    // Test correct head value
    EXPECT_EQ(
        NUMBER,
        llist->head->value
    );

    // Test correct tail value
    EXPECT_EQ(
        NUMBER,
        llist->tail->value
    );

    // TEST tail and head are the same
    EXPECT_EQ(
            llist->head->value,
            llist->tail->value
    );

    // Test pointers in tail and head
    EXPECT_EQ(
            NULL,
            llist->tail->next
    );
    EXPECT_EQ(
            NULL,
            llist->tail->previous
    );
    EXPECT_EQ(
            NULL,
            llist->head->next
    );
    EXPECT_EQ(
            NULL,
            llist->head->previous
    );

    // Clean memory
    free(llist->head);
    free(llist);
}

TEST_F(LlistTestFixture, test_clean_list) {

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
    );

    // Init list
    llist_init(&llist);

    // Add ten elements to the list
    for(int i = 0; i < 10; i++) {
        llist_append(llist, i);
    }

    // clean list
    llist_delete(&llist);

    EXPECT_EQ(
            NULL,
            llist
    );
}

TEST_F(LlistTestFixture, test_remove_one) {

    // Init list
    LList *llist;
    llist_init(&llist);

    // Add one number
    llist_append(llist, NUMBER);

    // Remove number
    llist_remove_one(llist, NUMBER);

    EXPECT_EQ(
        NULL,
        llist->head
    );

    EXPECT_EQ(
            NULL,
            llist->tail
    );


    // clean list
    llist_delete(&llist);
}





