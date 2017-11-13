#ifndef SHARED_POINTER_H
#define SHARED_POINTER_H

#include "llist.h"
#include "memory.h"

typedef struct {
    LListBaseItem base;
    void* addr;
    size_t references_count;
} SharedPointer;

void init_shared_pointer(LListBaseItem* item);
SharedPointer* find_shared_pointer(LList* list, void* addr);
SharedPointer* add_reference(LList* list, void* addr);
SharedPointer* remove_reference(LList* list, void* addr);

#endif // SHARED_POINTER_H
