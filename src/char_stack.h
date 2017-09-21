#ifndef _CHAR_STACK_H
#define _CHAR_STACK_H

#include <stdlib.h>
#include "memory.h

typedef struct char_stack_t CharStack;

/**
 * Initialization for char stack structure.
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
 * Pop char from given stack. Returns EOF if stack is empty.
 * @param stack target stack
 * @return poped char, EOF in case of empty stack
 */
int char_stack_pop(CharStack* stack);

#endif //_CHAR_STACK_H
