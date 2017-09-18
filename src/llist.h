#pragma once
#ifndef _LLIST_H
#define _LLIST_H

#include "debug.h"
#include "memory.h"

struct LListItem;

/**
 * @brief The LList structure contains the beginning and the end of linear list. An empty list
 * has null end and beginning.
 */
typedef struct LList {
    /// The beginning of list.
    struct LListItem* head;
    /// The end of list.
    struct LListItem* tail;
} LList;

/**
 * @brief The LListItem structure is single item of linear list which contains single integer
 * value.
 */
typedef struct LListItem {
    int value;
    struct LListItem* next;
    struct LListItem* previous;
} LListItem;

/**
 * @brief llist_init Allocates memory for list and null head and tail.
 * @param list Address of pointer which will point to llist structure.
 */
void llist_init(struct LList** list);

/**
 * @brief llist_append Appends one item with given value to the end of the list.
 * @param list List where item will be appended.
 * @param value Value of appended item.
 */
void llist_append(struct LList* list, int value);

/**
 * @brief llist_remove_one Remove first occurrence of item with given value.
 * @param list List where item will be searched.
 * @param value Value of deleting item.
 */
void llist_remove_one(struct LList* list, int value);

/**
 * @brief llist_delete Delete all items and list and set pointer to null.
 * @param list List which will be deleted.
 */
void llist_delete(struct LList** list);

#endif //_LLIST_H
