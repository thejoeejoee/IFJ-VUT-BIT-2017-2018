#include <stdio.h>
#include <memory.h>
#include "code_instruction.h"
#include "code_instruction_operand.h"
#include "debug.h"

void code_instruction_operand_free(CodeInstructionOperand** operand) {
    NULL_POINTER_CHECK(operand,);
    NULL_POINTER_CHECK(*operand,);
    // TODO: free operand with string data
}