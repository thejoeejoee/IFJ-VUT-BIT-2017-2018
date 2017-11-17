#include "code_generator.h"
#include "memory.h"
#include "common.h"
#include "debug.h"


void code_generator_register_signatures(const CodeGenerator* generator) {
    // flow control
    ADD_INSTRUCTION_SIGNATURE(I_MOVE, "MOVE", TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(I_CREATE_FRAME, "CREATEFRAME");
    ADD_INSTRUCTION_SIGNATURE(I_PUSH_FRAME, "PUSHFRAME");
    ADD_INSTRUCTION_SIGNATURE(I_POP_FRAME, "POPFRAME");
    ADD_INSTRUCTION_SIGNATURE(I_DEF_VAR, "DEFVAR", TYPE_INSTRUCTION_OPERAND_VARIABLE);
    ADD_INSTRUCTION_SIGNATURE(I_CALL, "CALL", TYPE_INSTRUCTION_OPERAND_LABEL);
    ADD_INSTRUCTION_SIGNATURE(I_RETURN, "RETURN");

    // stack
    ADD_INSTRUCTION_SIGNATURE(I_PUSH_STACK, "PUSHS", TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(I_POP_STACK, "POPS", TYPE_INSTRUCTION_OPERAND_VARIABLE);
    ADD_INSTRUCTION_SIGNATURE(I_CLEAR_STACK, "CLEARS");

    // arithmetic
    ADD_INSTRUCTION_SIGNATURE(
            I_ADD, "ADD",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_SUB, "SUB",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_MUL, "MUL",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_DIV, "DIV",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    // arithmetic stack
    ADD_INSTRUCTION_SIGNATURE(I_ADD_STACK, "ADDS");
    ADD_INSTRUCTION_SIGNATURE(I_SUB_STACK, "SUBS");
    ADD_INSTRUCTION_SIGNATURE(I_MUL_STACK, "MULS");
    ADD_INSTRUCTION_SIGNATURE(I_DIV_STACK, "DIVS");

    // comparing
    ADD_INSTRUCTION_SIGNATURE(
            I_LESSER_THEN, "LT",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_GREATER_THEN, "GT",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_EQUAL, "EQ",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);

    // comparing on stack
    ADD_INSTRUCTION_SIGNATURE(I_LESSER_THEN_STACK, "LTS");
    ADD_INSTRUCTION_SIGNATURE(I_GREATER_THEN_STACK, "GTS");
    ADD_INSTRUCTION_SIGNATURE(I_EQUAL_STACK, "EQS");

    // booleans
    ADD_INSTRUCTION_SIGNATURE(
            I_AND, "AND",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_OR, "OR",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_NOT, "NOT",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    // boolean on stack
    ADD_INSTRUCTION_SIGNATURE(I_AND_STACK, "ANDS");
    ADD_INSTRUCTION_SIGNATURE(I_OR_STACK, "ORS");
    ADD_INSTRUCTION_SIGNATURE(I_NOT_STACK, "NOTS");


    // conversions
    ADD_INSTRUCTION_SIGNATURE(
            I_INT_TO_FLOAT, "INT2FLOAT",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_FLOAT_TO_INT, "FLOAT2INT",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_FLOAT_ROUND_TO_EVEN_INT, "FLOAT2R2EINT",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_FLOAT_ROUND_TO_ODD_INT, "FLOAT2R2OINT",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_INT_TO_CHAR, "INT2CHAR",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_STRING_TO_INT, "STRI2INT",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);

    // stack conversions
    ADD_INSTRUCTION_SIGNATURE(I_INT_TO_FLOAT_STACK, "INT2FLOATS");
    ADD_INSTRUCTION_SIGNATURE(I_FLOAT_TO_INT_STACK, "FLOAT2INTS");
    ADD_INSTRUCTION_SIGNATURE(I_FLOAT_ROUND_TO_EVEN_INT_STACK, "FLOAT2R2EINTS");
    ADD_INSTRUCTION_SIGNATURE(I_FLOAT_ROUND_TO_ODD_INT_STACK, "FLOAT2R2OINTS");
    ADD_INSTRUCTION_SIGNATURE(I_INT_TO_CHAR_STACK, "INT2CHARS");
    ADD_INSTRUCTION_SIGNATURE(I_STRING_TO_INT_STACK, "STRI2INTS");

    // io
    ADD_INSTRUCTION_SIGNATURE(
            I_READ, "READ",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_DATA_TYPE);
    ADD_INSTRUCTION_SIGNATURE(
            I_WRITE, "WRITE",
            TYPE_INSTRUCTION_OPERAND_SYMBOL);

    // strings
    ADD_INSTRUCTION_SIGNATURE(
            I_CONCAT_STRING, "CONCAT",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_STRING_LENGTH, "STRLEN",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_GET_CHAR, "GETCHAR",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_SET_CHAR, "SETCHAR",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);

    // dynamic types
    ADD_INSTRUCTION_SIGNATURE(
            I_TYPE, "TYPE",
            TYPE_INSTRUCTION_OPERAND_VARIABLE, TYPE_INSTRUCTION_OPERAND_SYMBOL);

    // program flow manual
    ADD_INSTRUCTION_SIGNATURE(
            I_LABEL, "LABEL",
            TYPE_INSTRUCTION_OPERAND_LABEL);
    ADD_INSTRUCTION_SIGNATURE(
            I_JUMP, "JUMP",
            TYPE_INSTRUCTION_OPERAND_LABEL);
    ADD_INSTRUCTION_SIGNATURE(
            I_JUMP_IF_EQUAL, "JUMPIFEQ",
            TYPE_INSTRUCTION_OPERAND_LABEL, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_JUMP_IF_NOT_EQUAL, "JUMPIFNEQ",
            TYPE_INSTRUCTION_OPERAND_LABEL, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL);
    ADD_INSTRUCTION_SIGNATURE(
            I_JUMP_IF_EQUAL_STACK, "JUMPIFEQS",
            TYPE_INSTRUCTION_OPERAND_LABEL);
    ADD_INSTRUCTION_SIGNATURE(
            I_JUMP_IF_NOT_EQUAL_STACK, "JUMPIFNEQ",
            TYPE_INSTRUCTION_OPERAND_LABEL);

    // debug instructions
    ADD_INSTRUCTION_SIGNATURE(I_BREAK, "BREAK");
    ADD_INSTRUCTION_SIGNATURE(I_DEBUG_PRINT, "DPRINT", TYPE_INSTRUCTION_OPERAND_SYMBOL);
}

CodeGenerator* code_generator_init() {
    CodeGenerator* generator = memory_alloc(sizeof(CodeGenerator));

    generator->to_buffer = false;
    generator->first = generator->last = generator->buffer_last = generator->buffer_first = NULL;

    generator->instruction_signatures = memory_alloc(sizeof(CodeInstructionSignature) * I__LAST);

    for(int i = 0; i < I__LAST; ++i) {
        generator->instruction_signatures[i].type = I__NONE;
        generator->instruction_signatures[i].identifier = NULL;
    }

    code_generator_register_signatures(generator);

    return generator;
}

void code_generator_free(CodeGenerator** generator) {
    NULL_POINTER_CHECK(generator,);
    NULL_POINTER_CHECK(*generator,);
    CodeInstruction* instruction = (*generator)->first;
    CodeInstruction* next = NULL;
    while(instruction != NULL) {
        next = instruction->next;
        code_instruction_free(&instruction);
        instruction = next;
    }
    memory_free((*generator)->instruction_signatures);
    memory_free(*generator);
    *generator = NULL;
}

void code_generator_append_instruction(
        CodeGenerator* generator,
        CodeInstruction* instruction
) {
    NULL_POINTER_CHECK(generator,);
    NULL_POINTER_CHECK(instruction,);

    instruction->next = NULL;
    if(!generator->to_buffer) {
        if(generator->last == NULL) {
            generator->last = generator->first = instruction;
            instruction->prev = NULL;
        } else {
            generator->last->next = instruction;
            instruction->prev = generator->last;
            generator->last = instruction;
        }
    } else {
        if(generator->buffer_last == NULL) {
            generator->buffer_last = generator->buffer_first = instruction;
            instruction->prev = instruction->next = NULL;
        } else {
            generator->buffer_last->next = instruction;
            instruction->prev = generator->buffer_last;
            generator->buffer_last = instruction;
        }
    }
}

void code_generator_remove_instruction(CodeGenerator* generator, CodeInstruction* instruction)
{
    NULL_POINTER_CHECK(generator,);
    NULL_POINTER_CHECK(instruction,);

    if(generator->last == instruction)
        generator->last = instruction->prev;
    if(generator->first == instruction)
        generator->first = instruction->next;

    if(instruction->prev)
        instruction->prev->next = instruction->next;
    if(instruction->next)
        instruction->next->prev = instruction->prev;
    code_instruction_free(&instruction);
}

static bool _check_operand(CodeInstructionOperand* op, TypeInstructionOperand type) {
    if(type == 0) {
        return (op == NULL);
    } else {
        if(op == NULL) {
            LOG_WARNING("Required operand type of %d, NULL given.", type);
            return false;
        } else {
            if((op->type & type)) {
                return true;
            }
            LOG_WARNING("Expected operand type %d, given %d.", type, op->type);
            return false;
        }
    }
}


CodeInstruction* code_generator_new_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
) {
    NULL_POINTER_CHECK(generator, false);
    if((type_instruction < 0) || (type_instruction >= I__LAST)) {
        LOG_WARNING("Invalid type instruction.");
        return NULL;
    }

    CodeInstructionSignature* signature = generator->instruction_signatures + type_instruction;
    if(signature->type != type_instruction) {
        LOG_WARNING("Unknown type instruction %d.", type_instruction);
        return NULL;
    }

    if(!_check_operand(op0, signature->type0) ||
       !_check_operand(op1, signature->type1) ||
       !_check_operand(op2, signature->type2)) {
        LOG_WARNING(
                "Invalid operands to instruction %d: %d, %d, %d.",
                type_instruction,
                op0 != NULL ? op0->type : 0,
                op1 != NULL ? op1->type : 0,
                op2 != NULL ? op2->type : 0
        );
        return NULL;
    }

    CodeInstruction* instruction = code_instruction_init(type_instruction, op0, op1, op2);
    instruction->signature_buffer = signature;
    return instruction;
}

CodeInstruction* code_generator_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
) {
    NULL_POINTER_CHECK(generator, false);
    CodeInstruction* instruction = code_generator_new_instruction(
            generator,
            type_instruction,
            op0,
            op1,
            op2
    );
    if(instruction == NULL) {
        return NULL;
    }
    code_generator_append_instruction(
            generator,
            instruction
    );
    return instruction;
}

void code_generator_render(CodeGenerator* generator, FILE* file) {
    NULL_POINTER_CHECK(generator,);
    NULL_POINTER_CHECK(file,);

    // NOTE do optimalization

    char* rendered;
    CodeInstruction* instruction = generator->first;
    fprintf(file, ".IFJcode17\n");
    while(instruction != NULL) {
        rendered = code_instruction_render(instruction);
        NULL_POINTER_CHECK(rendered,);
        fprintf(file, "%s\n", rendered);
        instruction = instruction->next;
        memory_free(rendered);
    }
    fprintf(file, "\n");
}

void code_generator_insert_instruction_before(
        CodeGenerator* generator,
        CodeInstruction* to_insert,
        CodeInstruction* before_what
) {
    NULL_POINTER_CHECK(generator,);
    NULL_POINTER_CHECK(to_insert,);
    if(to_insert->next != NULL || to_insert->prev != NULL) {
        LOG_WARNING("Cannot insert already linked instruction.");
        return;
    }

    if(before_what == NULL) {
        to_insert->prev = NULL;
        to_insert->next = generator->first;
        if(generator->first != NULL) {
            generator->first->prev = to_insert;
            generator->first = to_insert;
        } else {
            generator->first = generator->last = to_insert;
        }
    } else {
        if(before_what->prev != NULL) {
            // not at start
            to_insert->prev = before_what->prev;
            to_insert->next = before_what;
            before_what->prev->next = to_insert;
        } else {
            // at start
            generator->first = to_insert;
            to_insert->prev = NULL;
            to_insert->next = before_what;
        }

        before_what->prev = to_insert;
    }

}

void code_generator_flush_buffer(CodeGenerator* generator) {
    NULL_POINTER_CHECK(generator,);

    if(generator->buffer_first != NULL) {
        generator->last->next = generator->buffer_first;
        generator->buffer_first->prev = generator->last;
        generator->last = generator->buffer_last;

        generator->buffer_last = generator->buffer_first = NULL;
    }
}
