#ifndef _CHAR_STACK_H
#define _CHAR_STACK_H

#include <stdlib.h>
#include "memory.h"

#define CHAR_STACK_MAX_LENGTH 16

/**
 * @brief Data structure representing the char stack
 */
typedef struct char_stack_t {
    short stack_head; // Index of head
    char stack[CHAR_STACK_MAX_LENGTH]; // Content of stack
} CharStack;

/**
 * @brief Initialization for char stack structure.
 * @return ptr to initialized
 */
CharStack* char_stack_init();

/**
 * Frees char stack from memory and sets target pointer to NULL.
 * @param stack
 */
void char_stack_free(CharStack** stack);

/**
 * Push given char to head of given stack.
 * @param stack target stack
 * @param c char to push
 * @return true, if was push successful
 */
bool char_stack_push(CharStack* stack, char c);

/**
 * @brief Make char stack empty
 *
 * @param stack
 */
void char_stack_empty(CharStack* stack);

/**
 * Pop char from given stack. Returns EOF if stack is empty.
 * @param stack target stack
 * @return poped char, EOF in case of empty stack
 */
int char_stack_pop(CharStack* stack);

#endif //_CHAR_STACK_H
