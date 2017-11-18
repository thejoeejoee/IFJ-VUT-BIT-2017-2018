#ifndef _CODE_GENERATOR_H
#define _CODE_GENERATOR_H

#include "debug.h"
#include "memory.h"
#include "code_instruction.h"

#define _INSTRUCTION_SIGNATURE_1(i_type, i_identifier) _INSTRUCTION_SIGNATURE_4(\
    i_type, \
    i_identifier, \
    TYPE_INSTRUCTION_OPERAND_NONE, \
    TYPE_INSTRUCTION_OPERAND_NONE, \
    TYPE_INSTRUCTION_OPERAND_NONE  \
)
#define _INSTRUCTION_SIGNATURE_2(i_type, i_identifier, type0) _INSTRUCTION_SIGNATURE_4(\
    i_type, \
    i_identifier, \
    type0, \
    TYPE_INSTRUCTION_OPERAND_NONE, \
    TYPE_INSTRUCTION_OPERAND_NONE  \
)
#define _INSTRUCTION_SIGNATURE_3(i_type, i_identifier, type0, type1) _INSTRUCTION_SIGNATURE_4(\
    i_type, \
    i_identifier, \
    type0, \
    type1, \
    TYPE_INSTRUCTION_OPERAND_NONE  \
)
#define _INSTRUCTION_SIGNATURE_4(i_type, i_identifier, type0_, type1_, type2_) do { \
    generator->instruction_signatures[i_type].type = (i_type); \
    generator->instruction_signatures[i_type].identifier = (i_identifier); \
    generator->instruction_signatures[i_type].type0 = (type0_); \
    generator->instruction_signatures[i_type].type1 = (type1_); \
    generator->instruction_signatures[i_type].type2 = (type2_); \
} while(0)

#define ADD_INSTRUCTION_SIGNATURE(...) MSVC_EXPAND(GET_OVERLOADED_MACRO12345(__VA_ARGS__, _INSTRUCTION_SIGNATURE_4, \
    _INSTRUCTION_SIGNATURE_3, _INSTRUCTION_SIGNATURE_2, _INSTRUCTION_SIGNATURE_1)(__VA_ARGS__))

typedef struct code_generator_t {
    CodeInstruction* first;
    CodeInstruction* last;

    CodeInstruction* buffer_first;
    CodeInstruction* buffer_last;
    bool to_buffer;

    CodeInstructionSignature* instruction_signatures;
} CodeGenerator;

/**
 * Constructor for generator.
 * @return new instance of generator
 */
CodeGenerator* code_generator_init();

/**
 * Destructor for generator.
 * @param generator instance
 */
void code_generator_free(CodeGenerator** generator);


/**
 * Constructor for CodeInstruction with operand validation.
 * @param generator instance
 * @param type_instruction type instruction
 * @param op0 first operand or NULL
 * @param op1 second operand or NULL
 * @param op2 third operand or NULL
 * @return initialized instruction or NULL in case of fail
 */
CodeInstruction* code_generator_new_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
);

void code_generator_remove_instruction(
        CodeGenerator* generator,
        CodeInstruction* instruction
);

void code_generator_insert_instruction_before(
        CodeGenerator* generator,
        CodeInstruction* to_insert,
        CodeInstruction* before_what
);

/**
 * Generic target for all generated methods for
 * @param generator instance
 * @param type_instruction type instruction
 * @param op0 optionally operand
 * @param type0 0 for forbidden operand, else bit mask for check op type
 * @param op1 optionally operand
 * @param type1 0 for forbidden operand, else bit mask for check op type
 * @param op2 optionally operand
 * @param type2 0 for forbidden operand, else bit mask for check op type
 * @internal
 * @return instance or NULL
 */
CodeInstruction* code_generator_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
);

/**
 * Raw method for adding new, already checked, instructions. Is controlled by 'to_buffer' flag,
 * which enable appending to temporary buffer, not to main list.
 * @param generator instance
 * @param type_instruction type of new instruction
 * @param op0 optionally operand
 * @param op1 optionally operand
 * @param op2 optionally operand
 * @internal
 */
void code_generator_append_instruction(
        CodeGenerator* generator,
        CodeInstruction* instruction
);


/**
 * Inserts instruction before another instruction. if before_what is NULL,
 * instruction is inserted at start of all instruction.
 * @param generator instance
 * @param to_insert instruction to insert
 * @param before_what before what insert?
 */
void code_generator_insert_instruction_before(
        CodeGenerator* generator,
        CodeInstruction* to_insert,
        CodeInstruction* before_what
);

/**
 * Flush temporary buffer to end of generated instruction.
 * @param generator instance
 */
void code_generator_flush_buffer(CodeGenerator* generator);


CodeInstruction* code_generator_last_instruction(CodeGenerator* generator);

/**
 * Render all generated instruction into given stream.
 * @param generator generator instance
 * @param file file to output
 */
void code_generator_render(CodeGenerator* generator, FILE* file);

#endif //_CODE_GENERATOR_H
