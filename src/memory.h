#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdlib.h>
#include <stdbool.h>
#include "debug.h"

#ifndef NDEBUG

#define INFO_MAX_LENGTH 64
#define INFO_FORMAT "%s:%d:%s()"

#define memory_alloc(size) memory_manager_malloc(size, __FILENAME__, __LINE__, __func__, NULL)
#define memory_free(address) memory_manager_free(address, NULL)

typedef struct memory_manager_page_t {
    void* address;
    size_t size;
    bool allocated;
    char* info;
    struct memory_manager_page_t* next;
} MemoryManagerPage;

typedef struct memory_manager_t {
    MemoryManagerPage* head;
} MemoryManager;

extern MemoryManager memory_manager;

void memory_manager_enter(MemoryManager* manager);

void memory_manager_exit(MemoryManager* manager);

#else

#define memory_alloc malloc
#define memory_free free
#define memory_manager_enter(...)
#define memory_manager_exit(...)

#endif

#endif //_MEMORY_H
