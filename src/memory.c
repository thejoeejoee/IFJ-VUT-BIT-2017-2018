#include "memory.h"

#ifndef NDEBUG
MemoryManager memory_manager;

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

    MemoryManagerPage* new_page = (MemoryManagerPage*) malloc(sizeof(MemoryManagerPage));
    NULL_POINTER_CHECK(new_page, NULL);

    new_page->next = NULL;
    new_page->size = size;
    new_page->allocated = true;
    new_page->info = (char*) malloc(INFO_MAX_LENGTH + 1);
    NULL_POINTER_CHECK(new_page->info, NULL);
    new_page->address = malloc(new_page->size);
    // TODO: is possible memory leak important to solve?
    NULL_POINTER_CHECK(new_page->address, NULL);

    snprintf(new_page->info, INFO_MAX_LENGTH, INFO_FORMAT, file, line, func);

    new_page->next = manager->head;
    manager->head = new_page;
    LOG_DEBUG("Successfully allocated %zu bytes for %s.", new_page->size, new_page->info);
    return new_page->address;
}

void memory_manager_free(void* address,
                         MemoryManager* manager) {
    NULL_POINTER_CHECK(address,);
    if (manager == NULL)
        manager = &memory_manager;

    MemoryManagerPage* page = manager->head;
    while ((page != NULL) && (page->address != address)) {
        page = page->next;
    }
    if (page == NULL) {
        LOG_WARNING("Allocated memory with address %p to free not found.", address);
        return;
    }
    page->allocated = false;
    free(page->address);
    page->address = NULL;
}

void memory_manager_enter(MemoryManager* manager) {
    LOG_DEBUG("Memory manager started.");
    if (manager == NULL)
        manager = &memory_manager;
    manager->head = NULL;
}

void memory_manager_exit(MemoryManager* manager) {
    if (manager == NULL)
        manager = &memory_manager;
    MemoryManagerPage* page = manager->head;
    MemoryManagerPage* next = NULL;
    int pages_count = 0;
    int size_sum = 0;

    while (page != NULL) {
        next = page->next;
        pages_count++;
        size_sum += page->size;

        if (page->allocated) {
            LOG_WARNING("Memory leak of %zu bytes from %s.", page->size, page->info);
            free(page->address);
        }
        free(page->info);
        free(page);
        page = next;
    }

    LOG_DEBUG(
            "Memory manager exited, %d pages freed of total size %d bytes.",
            pages_count,
            size_sum
    );
    manager->head = NULL;
}

#endif
