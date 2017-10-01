#include "code_generator.h"
#include "memory.h"

CodeGenerator* code_generator_init() {
    CodeGenerator* generator = memory_alloc(sizeof(CodeGenerator));

    generator->first = generator->last = NULL;

    return generator;
}

