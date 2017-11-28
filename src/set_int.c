#include "set_int.h"

SetInt* set_int_init()
{
    SetInt* set;
    llist_init(&set, sizeof(SetIntItem), NULL, NULL, NULL);

    return set;
}

void set_int_free(SetInt** set)
{
    NULL_POINTER_CHECK(set, );
    NULL_POINTER_CHECK(*set, );

    llist_free(set);
}

bool set_int_contains(SetInt* set, int value)
{
    NULL_POINTER_CHECK(set, false);

    SetIntItem* item = (SetIntItem*)set->head;

    while(item) {
        if(item->value == value)
            return true;
        item = (SetIntItem*)item->base.next;
    }

    return false;
}

bool set_int_is_empty(SetInt* set)
{
    NULL_POINTER_CHECK(set, true);

    return set->head == NULL;
}

size_t set_int_size(SetInt* set)
{
    NULL_POINTER_CHECK(set, 0);

    return llist_length(set);
}

void set_int_add(SetInt* set, int value)
{
    NULL_POINTER_CHECK(set, );

    if(set_int_contains(set, value))
        return;

    SetIntItem* new_item = (SetIntItem*)llist_new_tail_item(set);
    new_item->value = value;
}

void set_int_remove(SetInt* set, int value)
{
    NULL_POINTER_CHECK(set, );

    SetIntItem* item = (SetIntItem*)set->head;

    while(item != NULL) {
        if(item->value == value) {
            llist_remove_item(set, (LListBaseItem*)item);
            return;
        }
        item = (SetIntItem*)item->base.next;
    }
}

void set_int_clear(SetInt* set)
{
    NULL_POINTER_CHECK(set, );

    LListBaseItem* current_item = set->head;
    LListBaseItem* next_item = NULL;

    if(current_item != NULL) {
        do {
            next_item = current_item->next;
            memory_free(current_item);
            current_item = next_item;
        } while(current_item != NULL);
    }

    set->head = set->tail = NULL;
}

void set_int_print(SetInt* set)
{
    NULL_POINTER_CHECK(set, );

    SetIntItem* item = (SetIntItem*)set->head;
    printf("Set(");

    while(item != NULL) {
        printf("%d, ", item->value);
        item = (SetIntItem*)item->base.next;
    }
    printf(")");
}

void set_int_union(SetInt* set, SetInt* other)
{
    NULL_POINTER_CHECK(set, );
    NULL_POINTER_CHECK(other, );

    SetIntItem* item = (SetIntItem*)other->head;

    while(item != NULL) {
        set_int_add(set, item->value);
        item = (SetIntItem*)item->base.next;
    }
}

void set_int_difference(SetInt* set, SetInt* other)
{
    NULL_POINTER_CHECK(set, );
    NULL_POINTER_CHECK(other, );

    SetIntItem* item = (SetIntItem*)other->head;

    while(item != NULL) {
        set_int_remove(set, item->value);
        item = (SetIntItem*)item->base.next;
    }
}

SetInt* set_int_copy(SetInt* other)
{
    SetInt* new_set = set_int_init();
    set_int_union(new_set, other);

    return new_set;
}
