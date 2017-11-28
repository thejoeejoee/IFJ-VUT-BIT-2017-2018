#ifndef META_DATA_CODE_BLOCK_H
#define META_DATA_CODE_BLOCK_H

#include "oriented_graph.h"
#include "code_instruction.h"

typedef struct {
    GraphNodeBase base;
    bool direct_jump;
    size_t instructions_count;
    CodeInstruction* instructions;
    CodeInstruction* last_instruction;
    SetInt* conditional_jump;
} CodeBlock;

void init_code_block(GraphNodeBase* base);
void free_code_block(GraphNodeBase* base);
void code_block_set_start_instruction(CodeBlock* block, CodeInstruction* instruction);
void code_block_add_next_instruction(CodeBlock* block);

// INFO need to instrctuion which are consecutively
void code_block_add_instruction(CodeBlock* block, CodeInstruction* instruction);

#endif // META_DATA_CODE_BLOCK_H
