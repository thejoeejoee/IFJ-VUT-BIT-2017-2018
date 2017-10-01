#ifndef _CODE_GENERATOR_H
#define _CODE_GENERATOR_H

#include "instruction.h"

typedef struct code_generator_t {
    CodeInstruction* first;
    CodeInstruction* last;
} CodeGenerator;

CodeGenerator* code_generator_init();

#endif //_CODE_GENERATOR_H
