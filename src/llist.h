#pragma once
#ifndef _LLIST_H
#define _LLIST_H

#include "debug.h"
#include "memory.h"

struct LListItem;

/**
* @brief Function to free LListItem data
*/
typedef void (*llist_free_item_data)(void* data);

/**
* @brief Comparison function for comparing LListItem datas
* @return -1 if a < b, 0 if a == b, 1 if a > b
*/
typedef int (*llist_item_compare_function)(void* a, void* b);

/**
 * @brief The LList structure contains the beginning and the end of linear list. An empty list
 * has null end and beginning.
 */
typedef struct LList {
    /// The beginning of list.
    struct LListItem* head;
    /// The end of list.
    struct LListItem* tail;
    /// Function used to free item.
    llist_free_item_data free_function;
    llist_item_compare_function cmp_function;
} LList;

/**
 * @brief The LListItem structure is single item of linear list which contains single integer
 * value.
 */
typedef struct LListItem {
    void* value;
    struct LListItem* next;
    struct LListItem* previous;
} LListItem;

/**
 * @brief llist_init Allocates memory for list and null head and tail.
 * @param list Address of pointer which will point to llist structure.
 */
void llist_init(struct LList** list, llist_free_item_data free_function, llist_item_compare_function cmp_function);

/**
 * @brief llist_append Appends one item with given value to the end of the list.
 * @param list List where item will be appended.
 * @param value Value of appended item.
 */
void llist_append(struct LList* list, void* value);

/**
* @brief llist_pop_back Pops item from the end of the list.
* @param list List from which the item will be popped.
* @return Value of popped item.
*/
void* llist_pop_back(struct LList* list);

/**
* @brief llist_append Inserts one item with given value after item 'after' of the list.
* @param list List where item will be inserted.
* @param after Item from given list after which the item will be inserted.
* @param value Value of inserted item.
*/
void llist_insert_after(struct LList* list, struct LListItem* after, void* value);

/**
 * @brief llist_remove_one Remove first occurrence of item with given value.
 * @param list List where item will be searched.
 * @param value Value of deleting item.
 * @return false, when no item was removed.
 */
bool llist_remove_one(struct LList* list, void* value);

/**
 * @brief llist_free  all items and list and set pointer to null.
 * @param list List which will be d.
 */
void llist_free(struct LList** list);

#endif //_LLIST_H
