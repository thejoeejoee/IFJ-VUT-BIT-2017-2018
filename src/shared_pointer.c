#include "shared_pointer.h"

void init_shared_pointer(LListBaseItem* item)
{
    SharedPointer* v = (SharedPointer*)item;
    v->addr = NULL;
    v->references_count = 0;
}

SharedPointer* find_shared_pointer(LList* list, void* addr)
{
    NULL_POINTER_CHECK(list, NULL);
    NULL_POINTER_CHECK(addr, NULL);

    SharedPointer* result = NULL;
    SharedPointer* item = (SharedPointer*)list->head;

    while(item != NULL) {
        if(item->addr == addr)
            return item;
        item = (SharedPointer*)item->base.next;
    }

    return result;
}

SharedPointer* add_reference(LList* list, void* addr)
{
    SharedPointer* sh = find_shared_pointer(list, addr);
    if(sh != NULL) {
        sh->references_count++;
        return sh;
    }

    sh = (SharedPointer*)llist_new_tail_item(list);
    sh->addr = addr;
    sh->references_count++;
    return sh;
}

SharedPointer* remove_reference(LList* list, void* addr)
{
    SharedPointer* sh = find_shared_pointer(list, addr);
    if(sh == NULL)
        return NULL;

    sh->references_count--;
    if(sh->references_count == 0) {
        llist_remove_item(list, (LListBaseItem*)sh);
        return NULL;
    }

    return NULL;
}
