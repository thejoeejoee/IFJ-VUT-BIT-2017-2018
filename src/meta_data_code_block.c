#include "meta_data_code_block.h"

void init_code_block(GraphNodeBase* base)
{
    NULL_POINTER_CHECK(base, );

    CodeBlock* v = (CodeBlock*)base;
    v->instructions_count = 0;
    v->instructions = NULL;
    v->last_instruction = NULL;
    v->conditional_jump = set_int_init();
}

void free_code_block(GraphNodeBase* base)
{
    NULL_POINTER_CHECK(base, );

    CodeBlock* v = (CodeBlock*)base;
    set_int_free(&v->conditional_jump);
}

void code_block_set_start_instruction(CodeBlock* block, CodeInstruction* instruction)
{
    NULL_POINTER_CHECK(block, );
    NULL_POINTER_CHECK(instruction, );

    block->instructions = instruction;
    block->last_instruction = instruction;
    block->instructions_count++;
}

void code_block_add_next_instruction(CodeBlock* block)
{
    NULL_POINTER_CHECK(block, );
    if(block->instructions_count == 0) {
        LOG_WARNING("Bad adding instruction to code block");
        return;
    }

    block->instructions_count++;
    block->last_instruction = block->last_instruction->next;
}

void code_block_add_instruction(CodeBlock* block, CodeInstruction* instruction)
{
    NULL_POINTER_CHECK(block, );
    NULL_POINTER_CHECK(instruction, );

    if(block->instructions_count == 0)
        code_block_set_start_instruction(block, instruction);
    else
        code_block_add_next_instruction(block);
}
