
#include "llist.h"
#include <stdlib.h>

void llist_init(LList** list, size_t item_size, llist_init_item_data_callback_f init_function,
                llist_free_item_data_callback_f free_function, llist_item_compare_function cmp_function) {
    *list = (LList*) memory_alloc(sizeof(LList));

    llist_init_list(*list, item_size, init_function, free_function, cmp_function);
}

void llist_init_list(LList* list, size_t item_size, llist_init_item_data_callback_f init_function,
                     llist_free_item_data_callback_f free_function, llist_item_compare_function cmp_function) {
    list->head = NULL;
    list->tail = NULL;
    list->init_data_callback = init_function;
    list->free_data_callback = free_function;
    list->cmp_function = cmp_function;
    list->item_size = item_size;
}

LListBaseItem* llist_new_tail_item(LList* list) {
    NULL_POINTER_CHECK(list, NULL);

    LListBaseItem* new_item = (LListBaseItem*) memory_alloc(list->item_size);

    return llist_append_item(list, new_item);
}


LListBaseItem* llist_append_item(LList* list, LListBaseItem* new_item) {
    NULL_POINTER_CHECK(list, NULL);
    NULL_POINTER_CHECK(new_item, NULL);

    LListBaseItem* last_item = list->tail;

    if(list->head == NULL)
        list->head = new_item;

    if(list->init_data_callback != NULL)
        list->init_data_callback(new_item);

    if(last_item != NULL)
        last_item->next = new_item;
    new_item->previous = last_item;
    new_item->next = NULL;
    list->tail = new_item;

    return new_item;
}

LListBaseItem* llist_pop_back(LList* list) {
    LListBaseItem* tmp;

    if(list->tail == NULL) {
        return NULL;
    }
    tmp = list->tail;
    list->tail = list->tail->previous;
    if(list->tail == NULL) {
        list->head = NULL;
    }

    tmp->previous = NULL;
    return tmp;
}

void llist_insert_after(LList* list, LListBaseItem* after, LListBaseItem* new_item) {
    NULL_POINTER_CHECK(list,);
    NULL_POINTER_CHECK(after,);
    ASSERT(list->tail != NULL);

    if(after->next == NULL) {
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

LListBaseItem* llist_remove_item(LList* list, LListBaseItem* item) {
    NULL_POINTER_CHECK(list, NULL);
    NULL_POINTER_CHECK(item, NULL);

    LListBaseItem* next = item->next;

    if(item->previous == NULL) {
        if(list->head == item) {
            list->head = item->next;
        }
    } else {
        item->previous->next = item->next;
    }
    if(item->next == NULL) {
        if(list->tail == item) {
            list->tail = item->previous;
        }
    } else {
        item->next->previous = item->previous;
    }

    if(list->free_data_callback != NULL) {
        list->free_data_callback(item);
    }
    memory_free(item);

    return next;
}

void llist_free(LList** list) {
    NULL_POINTER_CHECK(list,);
    NULL_POINTER_CHECK(*list,);

    LListBaseItem* current_item = (*list)->head;
    LListBaseItem* next_item = NULL;

    if(current_item != NULL) {
        do {
            next_item = current_item->next;
            if((*list)->free_data_callback != NULL) {
                (*list)->free_data_callback(current_item);
            }
            memory_free(current_item);
            current_item = next_item;
        } while(current_item != NULL);
    }

    memory_free(*list);
    *list = NULL;
}

size_t llist_length(LList* list) {
    LListBaseItem* item = list->head;
    size_t count = 0;

    while(item != NULL) {
        item = item->next;
        count++;
    }

    return count;
}

LListBaseItem* llist_get_n_from_end(LList* list, size_t n) {
    LListBaseItem* item = list->tail;

    for(size_t i = 0; i < n; i++) {
        if(item == NULL)
            return NULL;

        item = item->previous;
    }

    return item;
}

void llist_item_str_init(LListBaseItem* item)
{
    NULL_POINTER_CHECK(item, );

    LListItemStr* v = (LListItemStr*) item;
    v->str = NULL;
}

void llist_item_str_free(LListBaseItem* item)
{
    NULL_POINTER_CHECK(item, );

    LListItemStr* v = (LListItemStr*) item;
    if(v->str != NULL)
        memory_free(v->str);
}
