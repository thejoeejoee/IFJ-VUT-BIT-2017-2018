#ifndef META_DATA_PEEP_HOLE_PATTERN_H
#define META_DATA_PEEP_HOLE_PATTERN_H

#include "llist.h"
#include "symtable.h"
#include "code_instruction.h"

typedef enum {
    META_PATTERN_FLAG_INVALID,
    META_PATTERN_FLAG_ALL,                      // any
    META_PATTERN_FLAG_STRING_EMPTY,             // ]
    META_PATTERN_FLAG_STRING,                   // [
    META_PATTERN_FLAG_VARIABLE,                 // !
    META_PATTERN_FLAG_INT_LITERAL_ZERO,         // }
    META_PATTERN_FLAG_INT_LITERAL,              // {
    META_PATTERN_FLAG_FLOAT_LITERAL_ZERO,       // )
    META_PATTERN_FLAG_FLOAT_LITERAL,            // (
    META_PATTERN_FLAG_BOOL_LITERAL,             // |
    META_PATTERN_FLAG_BOOL_LITERAL_TRUE,        // <
    META_PATTERN_FLAG_BOOL_LITERAL_FALSE,       // >
    META_PATTERN_FLAG_LABEL,                    // &
    META_PATTERN_FLAG_TEMP_VARIABLE_1,          // 1
    META_PATTERN_FLAG_TEMP_VARIABLE_2,          // 2
    META_PATTERN_FLAG_TEMP_VARIABLE_3,          // 3
    META_PATTERN_FLAG_TEMP_VARIABLE_4,          // 4
    META_PATTERN_FLAG_TEMP_VARIABLE_5           // 5
} MetaPHPatternFlag;

typedef struct peep_hole_pattern_t {
    LListBaseItem base;
    LList* matching_instructions;
    LList* replacement_instructions;
} PeepHolePattern;

typedef struct peep_hole_pattern_instruction_t {
    LListBaseItem base;
    TypeInstruction type;
    const char* op0_alias;
    const char* op1_alias;
    const char* op2_alias;

    int op0_occurrences_count;
    int op1_occurrences_count;
    int op2_occurrences_count;
} PeepHolePatternInstruction;

typedef struct mapped_operand_t {
    SymbolTableBaseItem base;
    CodeInstructionOperand* operand;
} MappedOperand;

MetaPHPatternFlag extract_flag(const char* alias);

// peep hole patterns sub item
void init_mapped_operand_item(SymbolTableBaseItem* item);
void free_mapped_operand_item(SymbolTableBaseItem* item);
void init_peep_hole_pattern(LListBaseItem* item);
void free_peep_hole_pattern(LListBaseItem* item);

void _code_optimizer_add_instruction_to_ph_pattern(LList* pattern_instruction_sub_list, TypeInstruction instruction, const char* op1_alias, const char* op2_alias, const char* op3_alias, int op0_occ_count, int op1_occ_count, int op2_occ_count);

void code_optimizer_add_matching_instruction_to_ph_pattern(PeepHolePattern* ph_pattern, TypeInstruction instruction, const char* op1_alias, const char* op2_alias, const char* op3_alias, int op0_occ_count, int op1_occ_count, int op2_occ_count);

void code_optimizer_add_replacement_instruction_to_ph_pattern(PeepHolePattern* ph_pattern, TypeInstruction instruction, const char* op1_alias, const char* op2_alias, const char* op3_alias);

#endif // META_DATA_PEEP_HOLE_PATTERN_H
