#include "gtest/gtest.h"

extern "C" {
#include "../src/llist.h"
}

struct TestListItem {
    LListBaseItem base;
    int value;
};

void free_item_int_data_func(LListBaseItem* item) {
    UNUSED(item);
}

int cmp_function(LListBaseItem* a, LListBaseItem* b) {
    TestListItem* A = (TestListItem*)a;
    TestListItem* B = (TestListItem*)b;
    if ( A->value < B->value ) {
        return -1;
    }
    else if ( A->value == B->value ) {
        return 0;
    }
    else {
        return 1;
    }
}

class LlistTestFixture : public ::testing::Test {
    protected:
        LList* llist;

        virtual void SetUp() {
            llist_init(&llist, sizeof(TestListItem), NULL, free_item_int_data_func, cmp_function);
        }

        virtual void TearDown() {
            if(llist != nullptr) {
                llist_free(&llist);
            }
        }
};

TEST_F(LlistTestFixture, Initialization) {

    /*
     * Test constructor
     */

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
}

TEST_F(LlistTestFixture, Destruction) {
    /*
     * Test destructor for list with one item
     */
    TestListItem* item = new TestListItem;
    item->value = 24;
    LList* llist;
    llist_init(&llist, sizeof(TestListItem), NULL, &free_item_int_data_func, &cmp_function);

    // Add one number
    llist_append_item(llist, (LListBaseItem*)item);

    // Remove all elements from list
    llist_free(&llist);

    // Test when list is free
    EXPECT_EQ(
            llist,
            nullptr
    ) << "Error free list with one item";

}

TEST_F(LlistTestFixture, DestructionWithMultipleItems) {

    /*
     * Test free for list with more than one item
     */
    LList* llist;
    llist_init(&llist, sizeof(TestListItem), NULL, &free_item_int_data_func, &cmp_function);

    // Add ten elements to the list
    for(int i = 0; i < 2; i++) {
        TestListItem* item = new TestListItem;
        item->value = 24;
        llist_append_item(llist, (LListBaseItem*)item);
    }

    // free list
    llist_free(&llist);

    EXPECT_EQ(
            llist,
            nullptr
    ) << "Error free list with more than one item";
}

TEST_F(LlistTestFixture, SimpleAppend) {
    constexpr int number = 42;

    TestListItem* item = new TestListItem;
    item->value = number;
    // Add one number
    llist_append_item(llist, (LListBaseItem*)item);

    // Test correct head value
    EXPECT_EQ(
            ((TestListItem*)llist->head)->value,
            number
    ) << "The value in head is incorrect";

    // Test correct tail value
    EXPECT_EQ(
            ((TestListItem*)llist->head)->value,
            number
    ) << "The value in tail is incorrect";

    // TEST tail and head are the same
    EXPECT_EQ(
            ((TestListItem*)llist->head)->value,
            ((TestListItem*)llist->tail)->value
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

}

TEST_F(LlistTestFixture, SimpleInsert) {
    std::vector<TestListItem*> items;
    for(int v: {42, 43, 44}) {
        TestListItem* newItem = new TestListItem;
        newItem->value = v;
        items.push_back(newItem);
    }

    llist_append_item(llist, (LListBaseItem*)items.at(0));

    // TEST tail and head are the same
    EXPECT_EQ(
        llist->head,
        llist->tail
    ) << "Pointers in head and tail are not the same";

    llist_insert_after(llist, llist->head, (LListBaseItem*)items.at(2));

    // TEST tail and head are ok
    EXPECT_EQ(
        llist->head,
        llist->tail->previous
    ) << "Pointers in head and tail are not ok";
    EXPECT_EQ(
        llist->head->next,
        llist->tail
    ) << "Pointers in head and tail are not ok";

    llist_insert_after(llist, llist->tail->previous, (LListBaseItem*)items.at(1));

    // Test correct list values
    EXPECT_EQ(
        ((TestListItem*)llist->head)->value,
        42
    ) << "The value in head is incorrect";

    EXPECT_EQ(
        ((TestListItem*)llist->head->next)->value,
        43
    ) << "The value in middle of the list is incorrect";

    EXPECT_EQ(
        ((TestListItem*)(llist->head->next->next))->value,
        44
    ) << "The value in tail is incorrect";

    // Test tail correct pointer
    EXPECT_EQ(
        llist->head->next->next,
        llist->tail
    ) << "The pointer in tail is incorrect";

}

TEST_F(LlistTestFixture, SimplePopBack) {
    std::vector<TestListItem*> items;
    for(int v: {42, 43}) {
        TestListItem* newItem = new TestListItem;
        newItem->value = v;
        items.push_back(newItem);
    }
    TestListItem* tmp;

    llist_append_item(llist, (LListBaseItem*)items.at(0));
    tmp = (TestListItem*)llist_pop_back(llist);
    // TEST return value of pop_back
    EXPECT_EQ(
        tmp->value,
        42
    ) << "Return value of pop_back is not ok";
    // TEST tail and head are ok
    EXPECT_EQ(
        llist->head,
        nullptr
    ) << "Pointers in head and tail are not ok";
    EXPECT_EQ(
        llist->tail,
        nullptr
    ) << "Pointers in head and tail are not ok";


    llist_append_item(llist, (LListBaseItem*)items.at(0));
    llist_append_item(llist, (LListBaseItem*)items.at(1));

    tmp = (TestListItem*)llist_pop_back(llist);
    // TEST return value of pop_back
    EXPECT_EQ(
        tmp->value,
        43
    ) << "Return value of pop_back is not ok";

    tmp = (TestListItem*)llist_pop_back(llist);
    // TEST return value of pop_back
    EXPECT_EQ(
        tmp->value,
        42
    ) << "Return value of pop_back is not ok";
    // TEST tail and head are ok
    EXPECT_EQ(
        llist->head,
        nullptr
    ) << "Pointers in head and tail are not ok";
    EXPECT_EQ(
        llist->tail,
        nullptr
    ) << "Pointers in head and tail are not ok";
}

TEST_F(LlistTestFixture, SimpleRemoveItem) {
    std::vector<TestListItem*> items;
    for(int v: {42, 43, 44}) {
        TestListItem* newItem = new TestListItem;
        newItem->value = v;
        items.push_back(newItem);
        llist_append_item(llist, (LListBaseItem*)newItem);
    }

    LListBaseItem* next;

    next = llist_remove_item(llist, llist->head->next);

    // TEST return value of llist_remove_item
    EXPECT_EQ(
        ((TestListItem*)next)->value,
        44
    ) << "Return value of llist_remove_item is not ok";
    // TEST head and tail values of llist
    EXPECT_EQ(
        ((TestListItem*)llist->head)->value,
        42
    ) << "Head value of llist is not ok";
    EXPECT_EQ(
        ((TestListItem*)llist->tail)->value,
        44
    ) << "Tail value of llist is not ok";

    next = llist_remove_item(llist, llist->head);

    // TEST return value of llist_remove_item
    EXPECT_EQ(
        ((TestListItem*)next)->value,
        44
    ) << "Return value of llist_remove_item is not ok";
    // TEST head and tail values of llist
    EXPECT_EQ(
        ((TestListItem*)llist->head)->value,
        44
    ) << "Head value of llist is not ok";
    EXPECT_EQ(
        ((TestListItem*)llist->tail)->value,
        44
    ) << "Tail value of llist is not ok";

    next = llist_remove_item(llist, llist->tail);

    // TEST return value of llist_remove_item
    EXPECT_EQ(
        next,
        nullptr
    ) << "Return value of llist_remove_item is not ok";
    // TEST head and tail values of llist
    EXPECT_EQ(
        llist->head,
        nullptr
    ) << "Head value of llist is not ok";
    EXPECT_EQ(
        llist->tail,
        nullptr
    ) << "Tail value of llist is not ok";
}

TEST_F(LlistTestFixture, SimpleRemove) {

    /*
     * Test remove one when llist have just one item
     */
    const int number = 42;

    TestListItem* item = new TestListItem;
    item->value = number;

    // Add one number
    llist_append_item(llist, (LListBaseItem*)item);

    // Remove number
    llist_remove_item(llist, (LListBaseItem*)(item));

    EXPECT_EQ(
            llist->head,
            nullptr
    ) << "Head is not NULL";

    EXPECT_EQ(
            llist->tail,
            nullptr
    ) << "Tail is not NULL";

}

TEST_F(LlistTestFixture, Length) {

    EXPECT_EQ(
            llist_length(llist),
            0
    ) << "Error in empty list length";

    llist_append_item(llist, (LListBaseItem*)(new TestListItem));
    llist_append_item(llist, (LListBaseItem*)(new TestListItem));
    llist_append_item(llist, (LListBaseItem*)(new TestListItem));

    EXPECT_EQ(
            llist_length(llist),
            3
    ) << "Error in list length";
}



