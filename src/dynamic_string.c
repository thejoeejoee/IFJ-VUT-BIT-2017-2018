#include "dynamic_string.h"
#include "memory.h"
#include "debug.h"
#include <string.h>


String* string_init_with_capacity(size_t initial_capacity) {
    ASSERT(initial_capacity >= 0);
    String* string = (String*) memory_alloc(sizeof(String));
    string->content = (char*) memory_alloc(sizeof(char) * initial_capacity + 1);
    string->content[0] = 0; //c-string terminator
    string->size = 1;
    string->capacity = initial_capacity + 1;
    return string;
}

void string_free(String** string) {
    NULL_POINTER_CHECK(string,);
    NULL_POINTER_CHECK(*string,);
    NULL_POINTER_CHECK((*string)->content,);

    memory_free((*string)->content);
    memory_free(*string);
    *string = NULL;
}

// helper function for allocating more space if needed
static inline void string_update_capacity(String* string, size_t needed_size) {
    NULL_POINTER_CHECK(string,)
    if(needed_size > string->capacity) {
        size_t new_capacity = 2 * string->capacity;
        new_capacity = new_capacity >= 2 ? new_capacity : 2;
        while(needed_size > new_capacity) {
            // impossible to handle
            new_capacity <<= 2; //multiply new capacity by 2
        }
        char* tmp = (char*) memory_alloc(sizeof(char) * new_capacity);
        strcpy(tmp, string->content);
        memory_free(string->content);
        string->capacity = new_capacity;
        string->content = tmp;
    }
}

void string_append_c(String* string, char c) {
    NULL_POINTER_CHECK(string,);
    size_t new_size = string->size + 1;
    string_update_capacity(string, new_size);
    ASSERT(string->capacity >= new_size);
    ASSERT(string->size >= 1);
    string->content[string->size - 1] = c;
    string->content[new_size - 1] = 0;
    string->size = new_size;
}

void string_append_s(String* string, const char* s) {
    NULL_POINTER_CHECK(string,);
    NULL_POINTER_CHECK(s,);
    size_t new_size = string->size + strlen(s);
    string_update_capacity(string, new_size);
    ASSERT(string->capacity >= new_size);
    ASSERT(string->size >= 1);
    strcpy(&string->content[string->size - 1], s);
    string->content[new_size - 1] = 0;
    string->size = new_size;
}

void string_append(String* string, const String* b) {
    NULL_POINTER_CHECK(string,);
    NULL_POINTER_CHECK(b,);
    size_t new_size = string->size + b->size - 1;
    string_update_capacity(string, new_size);
    ASSERT(string->capacity >= new_size);
    ASSERT(string->size >= 1);
    strcpy(&string->content[string->size - 1], b->content);
    string->content[new_size - 1] = 0;
    string->size = new_size;
}

void string_clear(String* string) {
    NULL_POINTER_CHECK(string,);
    ASSERT(string->capacity >= 1);
    string->size = 1;
    string->content[0] = 0;
}

char* string_content(String* string) {
    NULL_POINTER_CHECK(string, NULL);
    return string->content;
}

size_t string_length(String* string) { //excluding null terminator
    NULL_POINTER_CHECK(string, 0);
    ASSERT(string->size >= 1);
    return string->size - 1;
}

String* string_copy(String* source) {
    NULL_POINTER_CHECK(source, NULL);

    String* string = memory_alloc(sizeof(String));
    string->size = source->size;
    string->capacity = source->capacity;
    string->content = c_string_copy(source->content);
    return string;
}
