
#include "llist.h"
#include <stdlib.h>

void llist_init(LList** list, llist_free_item_data free_function, llist_item_compare_function cmp_function) {
    *list = (LList*) memory_alloc(sizeof(LList));
    (*list)->head = NULL;
    (*list)->tail = NULL;
    (*list)->free_function = free_function;
    (*list)->cmp_function = cmp_function;
}

void llist_append(LList* list, void* value) {
    NULL_POINTER_CHECK(list,);

    LListItem* new_item = (LListItem*) memory_alloc(sizeof(LListItem));
    LListItem* last_item = list->tail;

    if(list->head == NULL)
        list->head = new_item;

    new_item->value = value;

    if(last_item != NULL)
        last_item->next = new_item;
    new_item->previous = last_item;
    new_item->next = NULL;
    list->tail = new_item;
}

void* llist_pop_back(LList* list)
{
    LListItem* tmp;
    void* ret;

    if (list->tail == NULL) {
        return NULL;
    }
    tmp = list->tail;
    list->tail = list->tail->previous;
    if (list->tail == NULL) {
        list->head = NULL;
    }

    ret = tmp->value;

    if (list->free_function != NULL) {
        list->free_function(tmp->value);
    }
    memory_free(tmp);

    return ret;
}

void llist_insert_after(LList* list, LListItem* after, void* value)
{
    NULL_POINTER_CHECK(list, );
    NULL_POINTER_CHECK(after, );
    ASSERT(list->tail != NULL);

    LListItem* new_item = (LListItem*)memory_alloc(sizeof(LListItem));
    new_item->value = value;

    if (after->next == NULL) {
        ASSERT(after == list->tail);
        after->next = new_item;
        list->tail = new_item;
        new_item->previous = after;
        new_item->next = NULL;
    } else {
        new_item->next = after->next;
        new_item->previous = after;
        after->next->previous = new_item;
        after->next = new_item;
    }
}

bool llist_remove_one(LList* list, void* value) {
    NULL_POINTER_CHECK(list, false);
    if (list->cmp_function == NULL) { return false; }

    LListItem* current_item = list->head;
    if(current_item == NULL)
        return false;

    do {
        if(list->cmp_function(current_item->value,value) == 0) {
            LListItem* previous_item = current_item->previous;
            LListItem* next_item = current_item->next;

            if(current_item == list->head)      // check whether new head need to be set
                list->head = next_item;
            if(current_item == list->tail) // check whether new tail need to be set
                list->tail = previous_item;

            if(previous_item != NULL)
                previous_item->next = current_item->next;
            if(next_item != NULL)
                next_item->previous = current_item->previous;

            if (list->free_function != NULL) {
                list->free_function(current_item->value);
            }
            memory_free(current_item);
            return true;
        }
    } while((current_item = current_item->next) != NULL);
    return false;
}

void llist_free(LList** list) {
    NULL_POINTER_CHECK(list,);
    NULL_POINTER_CHECK(*list,);

    LListItem* current_item = (*list)->head;
    LListItem* next_item = NULL;

    if(current_item != NULL) {
        do {
            next_item = current_item->next;
            if ((*list)->free_function != NULL) {
                (*list)->free_function(current_item->value);
            }
            memory_free(current_item);
            current_item = next_item;
        } while(current_item != NULL);
    }

    memory_free(*list);
    *list = NULL;
}
