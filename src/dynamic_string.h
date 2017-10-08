#ifndef _DYNAMIC_STRING_H
#define _DYNAMIC_STRING_H

#include <stdlib.h>
#include "debug.h"

#define STRING_INITIAL_CAPACITY 128 // should be power of 2
#define string_init() string_init_with_capacity(STRING_INITIAL_CAPACITY)

typedef struct string_t {
    char* content; // only for internal usage
    size_t size; // only for internal usage, size of string data (including null terminator)
    size_t capacity; // only for internal usage, capacity for string data
} String;

/**
* @brief Creates and allocates new empty dynamic string (including null terminator) with custom capacity
*
* @param size_t* initial_capacity Initial capacity of dynamic string - should be power of 2
* @return String New empty dynamic string
*/
String* string_init_with_capacity(size_t initial_capacity);

/**
* @brief Deletes dynamic string and frees up it's allocated memory
*
* @param String* string Dynamic string to be deleted
*/
void string_free(String** string);

/**
* @brief Append character to dynamic string
*
* @param String* string Dynamic string to which we need to append.
* @param const char c Character to be appended.
*/
void string_append_c(String* string, char c);

/**
* @brief Append c string to dynamic string
*
* @param String* string Dynamic string to which we need to append.
* @param const char* s C string to be appended.
*/
void string_append_s(String* string, const char* s);

/**
* @brief Append dynamic string to dynamic string
*
* @param String* string Dynamic string to which we need to append.
* @param const String* b Dynamic string to be appended.
*/
void string_append(String* string, const String* b);

/**
* @brief Clear contents of dynamic string
*
* @param String* string Dynamic string to be cleared.
*/
void string_clear(String* string);

/**
* @brief Get dynamic string content
*
* @param String* string Dynamic string whose content we need.
* @return char* Content of dynamic string 'string' represented as c string.
*/
char* string_content(String* string);

/**
* @brief Get dynamic string length
*
* @param String* string Dynamic string whose length we need.
* @return size_t Length of dynamic string 'string'.
*/
size_t string_length(String* string);//excluding null terminator

/**
 * @brief Deep copy of string.
 * @param source source string instance
 * @return copied string instance
 */
String* string_copy(String* source);

#endif
