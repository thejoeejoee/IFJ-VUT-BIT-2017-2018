#include "common.h"
#include "memory.h"

char* c_string_copy(const char* string) {
    NULL_POINTER_CHECK(string, NULL);

    char* copied = memory_alloc(sizeof(char) * (strlen(string) + 1));

    MALLOC_CHECK(strcpy(copied, string));

    return copied;
}