#include "memory.h"

#ifdef NDEBUG

MemoryManager memory_manager;

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

    return (void*) (page + 1);
}

void memory_manager_free(void* address,
                         MemoryManager* manager) {
    NULL_POINTER_CHECK(address,);
    if (manager == NULL)
        manager = &memory_manager;

    MemoryManagerPage* page = manager->head;
    MemoryManagerPage* page_before = NULL;
    MemoryManagerPage* target_page = ((MemoryManagerPage*) address) - 1;
    while (page != NULL) {
        if (page == target_page)
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

void memory_manager_enter(MemoryManager* manager) {
    if (manager == NULL)
        manager = &memory_manager;
    if (manager->head != NULL) {
        LOG_WARNING("Try to enter already entered memory manager session.")
        return;
    }
    manager->head = NULL;
}

void memory_manager_exit(MemoryManager* manager) {
    if (manager == NULL)
        manager = &memory_manager;
    MemoryManagerPage* page = manager->head;
    MemoryManagerPage* next = NULL;

    while (page != NULL) {
        next = page->next;
        free(page);
        page = next;
    }
    manager->head = NULL;
}

#endif