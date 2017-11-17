#include "code_optimizer.h"
#include "memory.h"

CodeOptimizer* code_optimizer_init(CodeGenerator* generator)
{
    NULL_POINTER_CHECK(generator, NULL);

    CodeOptimizer* optimizer = memory_alloc(sizeof(CodeOptimizer));
    optimizer->variables_meta_data = symbol_table_init(32, sizeof(VariableMetaData), &init_variable_meta_data, NULL);

    optimizer->functions_meta_data = symbol_table_init(32, sizeof(FunctionMetaData), &init_function_meta_data, NULL);

    optimizer->generator = generator;
    optimizer->first_instruction = generator->first;

    return optimizer;
}

void code_optimizer_free(CodeOptimizer** optimizer)
{
    symbol_table_free((*optimizer)->variables_meta_data);
    symbol_table_free((*optimizer)->functions_meta_data);
    memory_free(*optimizer);
    *optimizer = NULL;
}

void code_optimizer_update_meta_data(CodeOptimizer* optimizer)
{
    // clear old statistic
    symbol_table_clear_buckets(optimizer->variables_meta_data);

    CodeInstruction* instruction = optimizer->first_instruction;
    const char* current_function = NULL;

    while(instruction != NULL) {
        if(instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_FUNCTION_START){
            ASSERT(instruction->type == I_LABEL);
            current_function = instruction->op0->data.label;
        }

        if(instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_FUNCTION_END)
            current_function = NULL;

        if(instruction->type == I_READ) {
            // TODO do something with variable meta data
            VariableMetaData* variable_meta_data =
                    code_optimizer_variable_meta_data(optimizer, instruction->op0[0].data.variable);
            variable_meta_data->purity_type |= META_TYPE_DYNAMIC_DEPENDENT;
        }

        code_optimizer_update_variable_meta_data(optimizer, instruction);
        code_optimizer_update_function_meta_data(optimizer, instruction, current_function);

        instruction = instruction->next;
    }

    // TODO another loop to spread dynamic
    instruction = optimizer->first_instruction;
    CodeInstruction* expr_start_instruction = NULL;
    while(instruction != NULL) {
        if(instruction->meta_data.type & CODE_INSTRUCTION_META_TYPE_EXPRESSION_START)
            expr_start_instruction = instruction;
        if(instruction->meta_data.type & CODE_INSTRUCTION_META_TYPE_EXPRESSION_END)
            expr_start_instruction = NULL;

        if(expr_start_instruction != NULL) {
            if(instruction->type == I_CALL) {
                FunctionMetaData* func_meta_data = code_optimizer_function_meta_data(optimizer, instruction->op0->data.label);
                expr_start_instruction->meta_data.purity_type |= func_meta_data->purity_type;
            }
        }

        instruction = instruction->next;
    }
}

void code_optimizer_update_function_meta_data(CodeOptimizer* optimizer, CodeInstruction* instruction, const char* current_func_label)
{
    if(current_func_label == NULL)
        return;

    MetaType flags = META_TYPE_PURE;
    if(instruction->type == I_READ)
        flags |= META_TYPE_DYNAMIC_DEPENDENT;
    else if(instruction->type == I_WRITE)
        flags |= META_TYPE_OUTPUTED;
    else  {
        if((instruction->type == I_READ ||
            instruction->type == I_MOVE ||
            instruction->type == I_POP_STACK) &&
                instruction->op0->type == TYPE_INSTRUCTION_OPERAND_VARIABLE &&
                instruction->op0->data.variable->frame == VARIABLE_FRAME_GLOBAL)
            flags |= META_TYPE_WITH_SIDE_EFFECT;
    }
    code_optimizer_function_meta_data(optimizer, current_func_label)->purity_type |= flags;
}

void code_optimizer_update_variable_meta_data(CodeOptimizer* optimizer, CodeInstruction* instruction)
{

    if(instruction->type == I_POP_STACK && instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_END)
        return;
    if(instruction->type == I_DEF_VAR)
        return;

    const size_t max_operands_count = 3;
    const CodeInstructionOperand* operands[] = {
        instruction->op0, instruction->op1, instruction->op2
    };

    for(size_t i = 0; i < max_operands_count; i++) {
        if(operands[i] == NULL)
            continue;
        if(operands[i]->type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
            continue;

        VariableMetaData* var_meta_data = code_optimizer_variable_meta_data(optimizer, operands[i]->data.variable);

        if(instruction->type == I_MOVE && i == 0) // it's first operand
            continue;

        var_meta_data->occurences_count++;
    }
}

void init_variable_meta_data(SymbolTableBaseItem* item)
{
    VariableMetaData* v = (VariableMetaData*)item;
    v->occurences_count = 0;
    v->purity_type = META_TYPE_PURE;
}


void init_function_meta_data(SymbolTableBaseItem* item)
{
    FunctionMetaData* v = (FunctionMetaData*)item;
    v->call_count = 0;
    v->purity_type = META_TYPE_PURE;
}


VariableMetaData* code_optimizer_variable_meta_data(CodeOptimizer* optimizer, SymbolVariable* variable)
{
    char* var_identifier = code_instruction_render_variable_identifier(variable);
    VariableMetaData* var_meta_data = ((VariableMetaData*)symbol_table_get_or_create(optimizer->variables_meta_data, var_identifier));
    memory_free(var_identifier);

    return var_meta_data;
}

FunctionMetaData*code_optimizer_function_meta_data(CodeOptimizer* optimizer, const char* key)
{
    return (FunctionMetaData*)symbol_table_get_or_create(optimizer->functions_meta_data, key);
}

bool code_optimizer_remove_unused_variables(CodeOptimizer* optimizer)
{
    CodeInstruction* instruction = optimizer->first_instruction;
    const size_t max_operands_count = 3;
    bool remove_something = false;
    MetaType expression_purity = META_TYPE_PURE;

    // null and analyze
    code_optimizer_update_meta_data(optimizer);

    // optimize
    instruction = optimizer->first_instruction;

    while(instruction) {
        bool delete_instruction = false;
        bool delete_expression = false;

        const CodeInstructionOperand* operands[] = {
            instruction->op0, instruction->op1, instruction->op2
        };

        if(instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_START)
            expression_purity |= instruction->meta_data.purity_type;

        for(size_t i = 0; i < max_operands_count; i++) {
            if(operands[i] == NULL || operands[i]->type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
                continue;

            SymbolVariable* variable = operands[i]->data.variable;
            const size_t variable_occurrences_count = code_optimizer_variable_meta_data(optimizer, variable)->occurences_count;

            if(variable_occurrences_count == 0 && variable->frame != VARIABLE_FRAME_TEMP) {
                if(instruction->type == I_POP_STACK && instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_END && expression_purity == META_TYPE_PURE)
                    delete_expression = true;
                else
                    delete_instruction = true;
                remove_something = true;
                break;
            }
        }

        if(delete_instruction) {
            CodeInstruction* temp = instruction->next;
            code_generator_remove_instruction(optimizer->generator, instruction);
            instruction = temp;
        }

        else if(delete_expression) {
            CodeInstruction* expr_instruction = instruction;
            CodeInstruction* prev_expr_instruction;

            while(expr_instruction->meta_data.type != CODE_INSTRUCTION_META_TYPE_EXPRESSION_START) {
                prev_expr_instruction = (CodeInstruction*)expr_instruction->prev;
                code_generator_remove_instruction(optimizer->generator, expr_instruction);
                expr_instruction = prev_expr_instruction;
            }

            instruction = expr_instruction->prev;
            code_generator_remove_instruction(optimizer->generator, expr_instruction);
        }

        else
            instruction = instruction->next;
    }

    optimizer->first_instruction = optimizer->generator->first;
    return remove_something;
}
