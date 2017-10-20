#pragma once
#ifndef _LLIST_H
#define _LLIST_H

#include "debug.h"
#include "memory.h"

struct LListBaseItem;
typedef struct LListBaseItem LListBaseItem;

/**
* @brief Function to free LListItem data
*/
typedef void (*llist_free_item_data)(LListBaseItem*);
typedef void (*llist_init_item_data)(LListBaseItem*);

/**
* @brief Comparison function for comparing LListItem datas
* @return -1 if a < b, 0 if a == b, 1 if a > b
*/
typedef int (*llist_item_compare_function)(LListBaseItem* a, LListBaseItem* b);

/**
 * @brief The LList structure contains the beginning and the end of linear list. An empty list
 * has null end and beginning.
 */
typedef struct LList {
    size_t item_size;
    /// The beginning of list.
    struct LListBaseItem* head;
    /// The end of list.
    struct LListBaseItem* tail;
    /// Function used to free item.
    llist_free_item_data free_function;
    llist_init_item_data init_function;
    llist_item_compare_function cmp_function;
} LList;

/**
 * @brief The LListItem structure is single item of linear list which contains single integer
 * value.
 */
typedef struct LListBaseItem {
    struct LListBaseItem* next;
    struct LListBaseItem* previous;
} LListBaseItem;

/**
 * @brief llist_init Allocates memory for list and null head and tail.
 * @param list Address of pointer which will point to llist structure.
 */
void llist_init(struct LList** list, size_t item_size, llist_init_item_data init_function, llist_free_item_data free_function, llist_item_compare_function cmp_function);

/**
 * @brief llist_append Appends one item with given value to the end of the list.
 * @param list List where item will be appended.
 * @param value Value of appended item.
 */
// TODO use init
LListBaseItem* llist_new_tail_item(struct LList* list);
LListBaseItem* llist_append_item(LList* list, LListBaseItem* new_item);

/**
* @brief llist_pop_back Pops item from the end of the list.
* @param list List from which the item will be popped.
* @return Value of popped item.
*/
LListBaseItem* llist_pop_back(struct LList* list);

/**
* @brief llist_append Inserts one item with given value after item 'after' of the list.
* @param list List where item will be inserted.
* @param after Item from given list after which the item will be inserted.
* @param value Value of inserted item.
*/
// TODO passed already created data
void llist_insert_after(struct LList* list, LListBaseItem* after, LListBaseItem* new_item);

/**
* @brief llist_remove_item Removes one item from the list.
* @param list List from which the item will be removed.
* @param item Item which will be removed.
* @return Next item after removed one.
*/
struct LListBaseItem* llist_remove_item(struct LList* list, struct LListBaseItem* item);

/**
 * @brief llist_free  all items and list and set pointer to null.
 * @param list List which will be d.
 */
void llist_free(struct LList** list);

/**
 * @brief llist_length Return items count in list.
 * @param list List whose items will be counted.
 * @return Items count in list.
 */
size_t llist_length(LList* list);

// TODO doc and test
LListBaseItem* llist_get_n_from_end(LList* list, size_t n);

#endif //_LLIST_H
