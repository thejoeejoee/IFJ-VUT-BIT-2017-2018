#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"

typedef struct debug_structure_t {
    int key;
    struct debug_structure_t * next;
} DebugStructure;

int main(int argc, char *argv[]) {
    DebugStructure a, b, c;
    a.key = 42;
    a.next = &b;
    b.next = &c;
    b.key = 38;
    c.next = NULL;
    c.key = 60;

    printf("Hello IFJ %d!\n", return_multiplied_answer_to_everything(1));
    return EXIT_SUCCESS;
}