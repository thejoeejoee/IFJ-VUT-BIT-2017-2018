#ifndef CODE_OPTIMIZER_H
#define CODE_OPTIMIZER_H

#include "code_instruction.h"
#include "code_generator.h"
#include "meta_data.h"
#include "meta_data_peep_hole_pattern.h"
#include "meta_data_code_block.h"
#include "oriented_graph.h"

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

    OrientedGraph* code_graph;
} CodeOptimizer;

bool code_optimizer_check_operand_with_meta_type_flag(CodeOptimizer* optimizer, CodeInstructionOperand* operand, MetaPHPatternFlag meta_type_flag);

VariableMetaData* code_optimizer_variable_meta_data(CodeOptimizer* optimizer, SymbolVariable* variable);

FunctionMetaData* code_optimizer_function_meta_data(CodeOptimizer* optimizer, const char* key);

LabelMetaData* code_optimizer_label_meta_data(CodeOptimizer* optimizer, const char* label);

// init/free
CodeOptimizer* code_optimizer_init(CodeGenerator* generator, SymbolVariable* temp1, SymbolVariable* temp2, SymbolVariable* temp3, SymbolVariable* temp4, SymbolVariable* temp5);

void code_optimizer_free(CodeOptimizer** optimizer);

// preparing code graph
void code_optimizer_split_code_to_graph(CodeOptimizer* optimizer);

// peep hole patterns managing
PeepHolePattern* code_optimizer_new_ph_pattern(CodeOptimizer* optimizer);

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
bool code_optimizer_remove_unused_variables(CodeOptimizer* optimizer, bool hard_remove);
bool code_optimizer_peep_hole_optimization(CodeOptimizer* optimizer);
bool code_optimizer_remove_unused_functions(CodeOptimizer* optimizer);

#endif // CODE_OPTIMIZER_H
