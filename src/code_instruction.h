#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H

#include <stdbool.h>
#include "code_instruction_operand.h"
#include "meta_data.h"

typedef enum type_instruction_t {
    // frame + function instructions

    I__NONE,

    I_MOVE,
    I_CREATE_FRAME,
    I_PUSH_FRAME,
    I_POP_FRAME,
    I_DEF_VAR,
    I_CALL,
    I_RETURN,

    // data stack instructions

    I_PUSH_STACK,
    I_POP_STACK,
    I_CLEAR_STACK,

    // arithmetic instructions

    I_ADD,
    I_SUB,
    I_MUL,
    I_DIV,
    I_ADD_STACK,
    I_SUB_STACK,
    I_MUL_STACK,
    I_DIV_STACK,

    // comparing

    I_LESSER_THEN,
    I_GREATER_THEN,
    I_EQUAL,
    I_LESSER_THEN_STACK,
    I_GREATER_THEN_STACK,
    I_EQUAL_STACK,

    // boolean logic

    I_AND,
    I_OR,
    I_NOT,
    I_AND_STACK,
    I_OR_STACK,
    I_NOT_STACK,

    // conversion

    I_INT_TO_FLOAT,
    I_FLOAT_TO_INT,
    I_FLOAT_ROUND_TO_EVEN_INT,
    I_FLOAT_ROUND_TO_ODD_INT,
    I_INT_TO_CHAR,
    I_STRING_TO_INT,
    I_INT_TO_FLOAT_STACK,
    I_FLOAT_TO_INT_STACK,
    I_FLOAT_ROUND_TO_EVEN_INT_STACK,
    I_FLOAT_ROUND_TO_ODD_INT_STACK,
    I_INT_TO_CHAR_STACK,
    I_STRING_TO_INT_STACK,

    // input output

    I_READ,
    I_WRITE,

    // strings

    I_CONCAT_STRING,
    I_STRING_LENGTH,
    I_GET_CHAR,
    I_SET_CHAR,

    // types manipulation

    I_TYPE,

    // program flow control

    I_LABEL,
    I_JUMP,
    I_JUMP_IF_EQUAL,
    I_JUMP_IF_NOT_EQUAL,
    I_JUMP_IF_EQUAL_STACK,
    I_JUMP_IF_NOT_EQUAL_STACK,

    // debug

    I_BREAK,
    I_DEBUG_PRINT,

    I__LAST
} TypeInstruction;

typedef enum {
    INSTRUCTION_TYPE_WRITE,
    INSTRUCTION_TYPE_DIRECT_JUMP,
    INSTRUCTION_TYPE_CONDITIONAL_JUMP,
    INSTRUCTION_TYPE_OTHER,
    INSTRUCTION_TYPE_INVALID,
    INSTRUCTION_TYPE_VAR_MODIFIERS,
} TypeInstructionClass;

typedef enum {
    CODE_INSTRUCTION_META_TYPE_NONE,
    CODE_INSTRUCTION_META_TYPE_EXPRESSION_START = 1,
    CODE_INSTRUCTION_META_TYPE_EXPRESSION_END = 2,
    CODE_INSTRUCTION_META_TYPE_FUNCTION_START = 4,
    CODE_INSTRUCTION_META_TYPE_FUNCTION_END = 8
} CodeInstructionMetaType;

typedef struct {
    CodeInstructionMetaType type;
    MetaType purity_type;
    bool interpretable;
    bool without_effect;
} CodeInstructionMetaData;

typedef struct code_instruction_signature_t {
    TypeInstruction type;
    const char* identifier;
    short operand_count;
    TypeInstructionOperand type0;
    TypeInstructionOperand type1;
    TypeInstructionOperand type2;
} CodeInstructionSignature;

typedef struct code_instruction_t {
    CodeInstructionMetaData meta_data;
    TypeInstruction type;
    CodeInstructionOperand* op0;
    CodeInstructionOperand* op1;
    CodeInstructionOperand* op2;

    struct code_instruction_t* next;
    struct code_instruction_t* prev;

    CodeInstructionSignature* signature_buffer;
} CodeInstruction;

/**
 * Constructor for instruction.
 * @param type type of instruction
 * @param op0 first operand
 * @param op1 second operand
 * @param op2 third operand
 * @return new instruction
 */
CodeInstruction* code_instruction_init(
        TypeInstruction type,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
);

/**
 * Frees instruction with operands.
 * @param instruction instance
 */
void code_instruction_free(CodeInstruction** instruction);

/**
 * Render instruction with operands.
 * @param instruction instance
 * @return rendered string
 */
char* code_instruction_render(CodeInstruction* instruction);

/**
 * Returns metaclass
 * @param instruction instruction to detect
 * @return class
 */
TypeInstructionClass instruction_class(CodeInstruction* instruction);

#endif //_INSTRUCTION_H
