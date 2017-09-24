#include "memory.h"

#ifdef NDEBUG

MemoryManager memory_manager;

#define _OFFSET_PAGE_TO_ADDRESS(page) ((void *) (((size_t) (page)) + sizeof(MemoryManagerPage)))
#define _OFFSET_ADDRESS_TO_PAGE(address) ((MemoryManagerPage *) (((size_t) (address)) - sizeof(MemoryManagerPage)))
#define memory_manager_enter(...)
#define memory_manager_exit(...)
#define memory_manager_log_stats(...)

void* memory_manager_malloc(
        size_t size,
        const char* file,
        unsigned line,
        const char* func,
        MemoryManager* manager
) {
    if (!size) {
        LOG_WARNING("Invalid size %zd .", size);
        return NULL;
    }
    if (manager == NULL)
        manager = &memory_manager;


    size_t total_size = sizeof(MemoryManagerPage) + size;

    MemoryManagerPage* page = (MemoryManagerPage*) malloc(total_size);

    MALLOC_CHECK(page);

    page->next = manager->head;
    manager->head = page;

    return _OFFSET_PAGE_TO_ADDRESS(page);
}

void memory_manager_free(void* address,
                         MemoryManager* manager) {
    NULL_POINTER_CHECK(address,);
    if (manager == NULL)
        manager = &memory_manager;

    MemoryManagerPage* page = manager->head;
    MemoryManagerPage* page_before = NULL;
    while (page != NULL) {
        if (page == _OFFSET_ADDRESS_TO_PAGE(address))
            break;

        page_before = page;
        page = page->next;
    }

    if (page == NULL) {
        LOG_WARNING("Allocated memory with address %p to free not found.", address);
        return;
    }
    if (page_before == NULL)
        manager->head = page->next;
    else
        page_before->next = page->next;

    free(page);
}

#endif