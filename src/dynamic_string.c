#include "dynamic_string.h"
#include "memory.h"
#include "debug.h"
#include <string.h>


String string_init_with_capacity(size_t initial_capacity) {
    ASSERT(initial_capacity > 1); //smaller capacity than 2 makes no sense
    String string;
    string.size = 1;
    string.content = (char*) memory_alloc(sizeof(char) * initial_capacity);
    // TODO: null pointer check
    if(string.content == NULL) {
        string.capacity = 0;
    } else {
        string.content[0] = 0; //c-string terminator
        string.capacity = initial_capacity;
    }
    return string;
}

void string_free(String* string) {
    NULL_POINTER_CHECK(string,)
    NULL_POINTER_CHECK(string->content,)

    memory_free(string->content);
}

// helper function for allocating more space if needed
static inline void string_update_capacity(String* string, size_t needed_size) {
    NULL_POINTER_CHECK(string,)
    if(needed_size > string->capacity) {
        size_t new_capacity = 2 * string->capacity;
        new_capacity = new_capacity >= 2 ? new_capacity : 2;
        while(needed_size > new_capacity) {
            new_capacity <<= 2; //multiply new capacity by 2
            // TODO: check overflow
        }
        // TODO: realloc, memory pool?
        char* tmp = (char*) memory_alloc(sizeof(char) * new_capacity);
        // TODO: check malloc
        strcpy(tmp, string->content);
        memory_free(string->content);
        string->capacity = new_capacity;
        string->content = tmp;
    }
}

void string_append_c(String* string, const char c) {
    size_t new_size = string->size + 1;
    string_update_capacity(string, new_size);
    ASSERT(string->capacity >= new_size);
    ASSERT(string->size >= 1);
    string->content[string->size - 1] = c;
    string->content[new_size - 1] = 0;
    string->size = new_size;
}

void string_append_s(String* string, const char* s) {
    size_t new_size = string->size + strlen(s);
    string_update_capacity(string, new_size);
    ASSERT(string->capacity >= new_size);
    ASSERT(string->size >= 1);
    strcpy(&string->content[string->size - 1], s);
    string->content[new_size - 1] = 0;
    string->size = new_size;
}

void string_append(String* string, const String* b) {
    size_t new_size = string->size + b->size - 1;
    string_update_capacity(string, new_size);
    ASSERT(string->capacity >= new_size);
    ASSERT(string->size >= 1);
    strcpy(&string->content[string->size - 1], b->content);
    string->content[new_size - 1] = 0;
    string->size = new_size;
}

// force creation of external symbol
extern inline void string_clear(String* string);
extern inline char* string_content(String* string);
extern inline size_t string_length(String* string);