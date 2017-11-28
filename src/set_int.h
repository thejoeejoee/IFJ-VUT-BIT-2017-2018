#ifndef SETINT_H
#define SETINT_H

#include "llist.h"

typedef struct {
    LListBaseItem base;
    int value;
} SetIntItem;

typedef LList SetInt;

SetInt* set_int_init();
void set_int_free(SetInt** set);

bool set_int_contains(SetInt* set, int value);
bool set_int_is_empty(SetInt* set);
size_t set_int_size(SetInt* set);

void set_int_add(SetInt* set, int value);
void set_int_remove(SetInt* set, int value);
void set_int_clear(SetInt* set);

void set_int_print(SetInt* set);
// TODO test
void set_int_union(SetInt* set, SetInt* other);

#endif // SETINT_H
