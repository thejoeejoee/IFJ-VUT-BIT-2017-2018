#ifndef CODE_OPTIMIZER_H
#define CODE_OPTIMIZER_H

#include "code_instruction.h"
#include "code_generator.h"
#include "symtable.h"
#include "meta_data.h"
#include "llist.h"

typedef struct code_optimizer_t {
    CodeGenerator* generator;
    SymbolTable* variables_meta_data;
    SymbolTable* functions_meta_data;
    SymbolTable* labels_meta_data;
    LList* peep_hole_patterns;
    SymbolVariable* temp1;
    SymbolVariable* temp2;
    SymbolVariable* temp3;
    SymbolVariable* temp4;
    SymbolVariable* temp5;
} CodeOptimizer;

typedef struct variable_meta_data_t {
    SymbolTableBaseItem base;
    int occurrences_count;
    MetaType purity_type;

    size_t read_usage_count;
} VariableMetaData;

typedef struct function_meta_data_t {
    SymbolTableBaseItem base;
    unsigned int call_count;
    MetaType purity_type;

    // has read
    // has write
    // has global mod
} FunctionMetaData;

typedef struct label_meta_data_t {
    SymbolTableBaseItem base;
    int occurrences_count;
} LabelMetaData;

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

typedef struct mapped_operand_t {
    SymbolTableBaseItem base;
    CodeInstructionOperand* operand;
} MappedOperand;

MetaPHPatternFlag extract_flag(const char* alias);
bool code_optimizer_check_operand_with_meta_type_flag(CodeOptimizer* optimizer, CodeInstructionOperand* operand, MetaPHPatternFlag meta_type_flag);

// meta data sub item
void init_variable_meta_data(SymbolTableBaseItem* item);

void init_function_meta_data(SymbolTableBaseItem* item);

void init_label_meta_data(SymbolTableBaseItem* item);

VariableMetaData* code_optimizer_variable_meta_data(CodeOptimizer* optimizer, SymbolVariable* variable);

FunctionMetaData* code_optimizer_function_meta_data(CodeOptimizer* optimizer, const char* key);

LabelMetaData* code_optimizer_label_meta_data(CodeOptimizer* optimizer, const char* label);

// init/free
CodeOptimizer* code_optimizer_init(CodeGenerator* generator, SymbolVariable* temp1, SymbolVariable* temp2, SymbolVariable* temp3, SymbolVariable* temp4, SymbolVariable* temp5);

void code_optimizer_free(CodeOptimizer** optimizer);

// peep hole patterns sub item
void init_mapped_operand_item(SymbolTableBaseItem* item);
void free_mapped_operand_item(SymbolTableBaseItem* item);
void init_peep_hole_pattern(LListBaseItem* item);
void free_peep_hole_pattern(LListBaseItem* item);

// peep hole patterns managing
PeepHolePattern* code_optimizer_new_ph_pattern(CodeOptimizer* optimizer);

void _code_optimizer_add_instruction_to_ph_pattern(LList* pattern_instruction_sub_list, TypeInstruction instruction, const char* op1_alias, const char* op2_alias, const char* op3_alias, int op0_occ_count, int op1_occ_count, int op2_occ_count);

void code_optimizer_add_matching_instruction_to_ph_pattern(PeepHolePattern* ph_pattern, TypeInstruction instruction, const char* op1_alias, const char* op2_alias, const char* op3_alias, int op0_occ_count, int op1_occ_count, int op2_occ_count);

void code_optimizer_add_replacement_instruction_to_ph_pattern(PeepHolePattern* ph_pattern, TypeInstruction instruction, const char* op1_alias, const char* op2_alias, const char* op3_alias);

SymbolTable* code_optimizer_check_ph_pattern(CodeOptimizer* optimizer, PeepHolePattern* ph_pattern, CodeInstruction* instruction);

CodeInstruction* code_optimizer_new_instruction_with_mapped_operands(CodeOptimizer* optimizer, PeepHolePatternInstruction*  ph_pattern_instruction, SymbolTable* mapped_operands);

// updating meta data
void code_optimizer_adding_instruction(CodeOptimizer* optimizer, CodeInstruction* instruction);
void code_optimizer_removing_instruction(CodeOptimizer* optimizer, CodeInstruction* instruction);

void code_optimizer_update_meta_data(CodeOptimizer* optimizer);

void code_optimizer_update_function_meta_data(CodeOptimizer* optimizer, CodeInstruction* instruction,
                                              const char* current_func_label);

void code_optimizer_update_label_meta_data(CodeOptimizer* optimizer, CodeInstruction* instruction);

// optimizing functions
bool code_optimizer_remove_unused_variables(CodeOptimizer* optimizer);
bool code_optimizer_peep_hole_optimization(CodeOptimizer* optimizer);
bool code_optimizer_remove_unused_functions(CodeOptimizer* optimizer);

#endif // CODE_OPTIMIZER_H
