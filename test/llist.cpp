#include "gtest/gtest.h"

extern "C" {
#include "../src/llist.h"
}

void free_item_int_data_func(void* data) {
    if(data != NULL)
        memory_free(data);
}

int cmp_function(void* a, void* b) {
    int A = *((int*) a);
    int B = *((int*) b);
    if(A < B) {
        return -1;
    } else if(A == B) {
        return 0;
    } else {
        return 1;
    }
}

class LlistTestFixture : public ::testing::Test {
    protected:
        LList* llist;

        virtual void SetUp() {
            llist_init(&llist, free_item_int_data_func, cmp_function);
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

    // Init seznam
    LList* llist;
    llist_init(&llist, NULL, NULL);

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

    // Free memory
    llist_free(&llist);
}

TEST_F(LlistTestFixture, Destruction) {
    /*
     * Test destructor for list with one item
     */
    int* data = (int*) memory_alloc(sizeof(int));
    *data = 24;
    // Add one number
    llist_append(llist, data);

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

    // Add ten elements to the list
    for(int i = 0; i < 2; i++) {
        int* data = (int*) memory_alloc(sizeof(int));
        *data = i;
        llist_append(llist, data);
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

    int* data = (int*) memory_alloc(sizeof(int));
    *data = number;
    // Add one number
    llist_append(llist, data);

    // Test correct head value
    EXPECT_EQ(
            *((int*) llist->head->value),
            number
    ) << "The value in head is incorrect";

    // Test correct tail value
    EXPECT_EQ(
            *((int*) llist->tail->value),
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

}

TEST_F(LlistTestFixture, SimpleInsert) {
    int* data[3];
    data[0] = (int*) memory_alloc(sizeof(int));
    data[1] = (int*) memory_alloc(sizeof(int));
    data[2] = (int*) memory_alloc(sizeof(int));

    *data[0] = 42;
    *data[1] = 43;
    *data[2] = 44;

    llist_append(llist, data[0]);

    // TEST tail and head are the same
    EXPECT_EQ(
            llist->head,
            llist->tail
    ) << "Pointers in head and tail are not the same";

    llist_insert_after(llist, llist->head, data[2]);

    // TEST tail and head are ok
    EXPECT_EQ(
            llist->head,
            llist->tail->previous
    ) << "Pointers in head and tail are not ok";
    EXPECT_EQ(
            llist->head->next,
            llist->tail
    ) << "Pointers in head and tail are not ok";

    llist_insert_after(llist, llist->tail->previous, data[1]);

    // Test correct list values
    EXPECT_EQ(
            *((int*) llist->head->value),
            42
    ) << "The value in head is incorrect";

    EXPECT_EQ(
            *((int*) llist->head->next->value),
            43
    ) << "The value in middle of the list is incorrect";

    EXPECT_EQ(
            *((int*) llist->head->next->next->value),
            44
    ) << "The value in tail is incorrect";

    // Test tail correct pointer
    EXPECT_EQ(
            llist->head->next->next,
            llist->tail
    ) << "The pointer in tail is incorrect";

}

TEST_F(LlistTestFixture, SimplePopBack) {
    int* data[2];
    int* tmp;
    data[0] = (int*) memory_alloc(sizeof(int));
    data[1] = (int*) memory_alloc(sizeof(int));

    *data[0] = 42;
    *data[1] = 43;

    llist_append(llist, data[0]);
    tmp = (int*) llist_pop_back(llist);
    // TEST return value of pop_back
    EXPECT_EQ(
            *tmp,
            *data[0]
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


    llist_append(llist, data[0]);
    llist_append(llist, data[1]);

    tmp = (int*) llist_pop_back(llist);
    // TEST return value of pop_back
    EXPECT_EQ(
            *tmp,
            *data[1]
    ) << "Return value of pop_back is not ok";

    tmp = (int*) llist_pop_back(llist);
    // TEST return value of pop_back
    EXPECT_EQ(
            *tmp,
            *data[0]
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

    memory_free(data[0]);
    memory_free(data[1]);
}

TEST_F(LlistTestFixture, SimpleRemoveItem) {
    LListItem* next;
    int* data[3];
    data[0] = (int*) memory_alloc(sizeof(int));
    data[1] = (int*) memory_alloc(sizeof(int));
    data[2] = (int*) memory_alloc(sizeof(int));

    *data[0] = 42;
    *data[1] = 43;
    *data[2] = 44;

    llist_append(llist, data[0]);
    llist_append(llist, data[1]);
    llist_append(llist, data[2]);

    next = llist_remove_item(llist, llist->head->next);

    // TEST return value of llist_remove_item
    EXPECT_EQ(
            *((int*) next->value),
            44
    ) << "Return value of llist_remove_item is not ok";
    // TEST head and tail values of llist
    EXPECT_EQ(
            *((int*) llist->head->value),
            42
    ) << "Head value of llist is not ok";
    EXPECT_EQ(
            *((int*) llist->tail->value),
            44
    ) << "Tail value of llist is not ok";

    next = llist_remove_item(llist, llist->head);

    // TEST return value of llist_remove_item
    EXPECT_EQ(
            *((int*) next->value),
            44
    ) << "Return value of llist_remove_item is not ok";
    // TEST head and tail values of llist
    EXPECT_EQ(
            *((int*) llist->head->value),
            44
    ) << "Head value of llist is not ok";
    EXPECT_EQ(
            *((int*) llist->tail->value),
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

    int* data = (int*) memory_alloc(sizeof(int));
    *data = number;

    // Add one number
    llist_append(llist, data);

    // Remove number
    llist_remove_one(llist, (void*) (&number));

    EXPECT_EQ(
            llist->head,
            nullptr
    ) << "Head is not NULL";

    EXPECT_EQ(
            llist->tail,
            nullptr
    ) << "Tail is not NULL";

}

TEST_F(LlistTestFixture, MultipleRemove) {

    /*
     * Test remove one when llist have more than one item
     */

    // Add testing data to the list
    for(int i = 0; i <= 2; i++) {
        int* data = (int*) memory_alloc(sizeof(int));
        *data = i;
        llist_append(llist, data);
    }

    // Remove items with value 2 & 4
    int test = 1;
    llist_remove_one(llist, &test);

    // Get two items from llist
    LListItem* first = llist->head;
    LListItem* second = first->next;

    // Test values in items
    EXPECT_EQ(
            *((int*) first->value),
            0
    ) << "Error value in first item";

    EXPECT_EQ(
            *((int*) second->value),
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

TEST_F(LlistTestFixture, Length) {

    EXPECT_EQ(
            llist_length(llist),
            0
    ) << "Error in empty list length";

    llist_append(llist, nullptr);
    llist_append(llist, nullptr);
    llist_append(llist, nullptr);

    EXPECT_EQ(
            llist_length(llist),
            3
    ) << "Error in list length";
}



