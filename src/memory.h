#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdlib.h>
#include <stdbool.h>
#include "common.h"

/**
 * PUBLIC API FOR MEMORY MANAGER:
 *
 * ptr = memory_alloc(size)
 * memory_free(ptr)
 *
 * Based on NDEBUG - if defined, both functions are mapped to stdlib.
 * If not defined, global memory managed handles functions.
 *
 * memory_manager_enter and memory_manager_exit starts and closes memory session.
 * Exit also frees all non-freed and warns about all memory leaks.
 */
#ifndef NDEBUG

#define memory_alloc_2(size, manager) memory_manager_malloc(size, __FILENAME__, __LINE__, __func__, manager)
#define memory_alloc_1(size) memory_manager_malloc(size, __FILENAME__, __LINE__, __func__, NULL)
#define memory_alloc(args...) GET_OVERLOADED_MACRO12(args, memory_alloc_2, memory_alloc_1)(args)

#define memory_free_1(address) memory_manager_free(address, NULL)
#define memory_free_2(address, manager) memory_manager_free(address, manager)
#define memory_free(args...) GET_OVERLOADED_MACRO12(args, memory_free_2, memory_free_1)(args)


#define INFO_MAX_LENGTH 64
#define INFO_FORMAT "%s:%d:%s()"

/**
 * @brief Memory page as one unit of allocated memory. Stored also info about place of allocation, size and
 * flag for state of freeing. Works as linked list, with next linked pages.
 */
typedef struct memory_manager_page_t {
    void* address;
    size_t size;
    bool allocated;
    char* info;
    struct memory_manager_page_t* next;
} MemoryManagerPage;

/**
 * @brief Memory manager actually holds only header of linked list of pages.
 */
typedef struct memory_manager_t {
    MemoryManagerPage* head;
} MemoryManager;

extern MemoryManager memory_manager;

/**
 * Enters memory manager session for given manager.
 * @param manager optional specified memory manager
 */
void memory_manager_enter(MemoryManager* manager);

/**
 * Exits session of given manager. Also deallocate all pages and non-freed memory blocks.
 * For non-freed logs warnings for possible memory leaks.
 * @param manager optional specified memory manager
 */
void memory_manager_exit(MemoryManager* manager);

/**
 * Replacement for malloc, which uses memory manager with pages and stored info about allocation.
 * @param size size of wanted memory
 * @param file source file of allocation
 * @param line line in file of allocation
 * @param func function of allocation
 * @param manager optionally custom manager
 * @return pointer to allocated memory block
 */
void* memory_manager_malloc(
        size_t size,
        const char* file,
        unsigned line,
        const char* func,
        MemoryManager* manager
);

/**
 * Replacement for free, mark page as freed
 * @param address pointer to memory block to free
 * @param manager memory manager
 */
void memory_manager_free(
        void* address,
        MemoryManager* manager
);

/**
 * Log stats about actual session of memory manager.
 * @param manager optional memory manager
 */
void memory_manager_log_stats(MemoryManager* manager);

#else

/**
* Stdlib callbacks for memory management functions.
*/
#define memory_alloc(size, ...) malloc(size)
#define memory_free free(addr, ...) free(addr)
#define memory_manager_enter(...)
#define memory_manager_exit(...)
#define memory_manager_log_stats(...)

#endif

#endif //_MEMORY_H
