#ifndef CODE_OPTIMIZER_H
#define CODE_OPTIMIZER_H

#include "code_instruction.h"
#include "code_generator.h"
#include "symtable.h"
#include "meta_data.h"

typedef struct code_optimizer_t {
    CodeInstruction* first_instruction;
    CodeGenerator* generator;
    SymbolTable* variables_meta_data;
    SymbolTable* functions_meta_data;
} CodeOptimizer;

// TODO move to .c?
typedef struct variable_meta_data_t {
    SymbolTableBaseItem base;
    unsigned int occurences_count;
    MetaType purity_type;
} VariableMetaData;

typedef struct function_meta_data_t {
    SymbolTableBaseItem base;
    unsigned int call_count;
    MetaType purity_type;
} FunctionMetaData;

void init_variable_meta_data(SymbolTableBaseItem* item);
void init_function_meta_data(SymbolTableBaseItem* item);
VariableMetaData* code_optimizer_variable_meta_data(CodeOptimizer* optimizer, SymbolVariable* variable);
FunctionMetaData* code_optimizer_function_meta_data(CodeOptimizer* optimizer, const char* key);

CodeOptimizer* code_optimizer_init(CodeGenerator* generator);
void code_optimizer_free(CodeOptimizer** optimizer);

void code_optimizer_update_meta_data(CodeOptimizer* optimizer);

void code_optimizer_update_variable_meta_data(CodeOptimizer* optimizer, CodeInstruction* instruction);
void code_optimizer_update_function_meta_data(CodeOptimizer* optimizer, CodeInstruction* instruction, const char* current_func_label);

bool code_optimizer_remove_unused_variables(CodeOptimizer* optimizer);

#endif // CODE_OPTIMIZER_H
