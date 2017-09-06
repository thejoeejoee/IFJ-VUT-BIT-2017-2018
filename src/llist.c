//
// Created by ns288774 on 5.9.2017.
//

#include "llist.h"
#include <stdlib.h>

void llist_init(struct LList** list) {
    *list = (struct LList*)malloc(sizeof(struct LList));
    (*list)->head = NULL;
    (*list)->tail = NULL;
}

void llist_append(struct LList* list, int value) {
    struct LListItem* new_item = (struct LListItem*)malloc(sizeof(struct LListItem));
    struct LListItem* last_item = list->tail;

    if(list->head == NULL)
        list->head = new_item;

    new_item->value = value;

    if(last_item != NULL)
        last_item->next = new_item;
    new_item->previous = last_item;
    new_item->next = NULL;
    list->tail = new_item;
}

void llist_remove_one(struct LList* list, int value) {
    struct LListItem* current_item = list->head;
    if(current_item == NULL)
        return;

    do {
        if(current_item->value == value) {
            struct LListItem* previous_item = current_item->previous;
            struct LListItem* next_item = current_item->next;

            if(current_item == list->head)      // check whether new head need to be set
                list->head = next_item;
            if(current_item == list->tail) // check whether new tail need to be set
                list->tail = previous_item;

            if(previous_item != NULL)
                previous_item->next = current_item->next;
            if(next_item != NULL)
                next_item->previous = current_item->previous;

            free(current_item);
            break;
        }
    } while((current_item = current_item->next) != NULL);
}

void llist_delete(struct LList** list) {
    struct LListItem* current_item = (*list)->head;
    struct LListItem* next_item = NULL;

    if(current_item != NULL) {
        do {
            next_item = current_item->next;
            free(current_item);
            current_item = next_item;
        } while(current_item != NULL);
    }

    free(*list);
    *list = NULL;
}
