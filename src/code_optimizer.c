#include "code_optimizer.h"
#include "memory.h"

CodeOptimizer* code_optimizer_init(CodeGenerator* generator)
{
    NULL_POINTER_CHECK(generator, NULL);

    CodeOptimizer* optimizer = memory_alloc(sizeof(CodeOptimizer));
    optimizer->variables_meta_data = symbol_table_init(32, sizeof(VariableMetaData), &init_variable_meta_data, NULL);
    optimizer->generator = generator;
    optimizer->first_instruction = generator->first;

    return optimizer;
}

void code_optimizer_free(CodeOptimizer** optimizer)
{
    symbol_table_free((*optimizer)->variables_meta_data);
    memory_free(*optimizer);
    *optimizer = NULL;
}

void code_optimizer_update_variables_meta_data(CodeOptimizer* optimizer)
{
    // clear old statistic
    symbol_table_clear_buckets(optimizer->variables_meta_data);

    CodeInstruction* instruction = optimizer->first_instruction;
    const size_t max_operands_count = 3;

    while(instruction != NULL) {
        const CodeInstructionOperand* operands[] = {
            instruction->op0, instruction->op1, instruction->op2
        };

        for(size_t i = 0; i < max_operands_count; i++) {
            if(operands[i] == NULL || operands[i]->type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
                continue;
            char* var_identifier = code_instruction_render_variable_identifier(operands[i]->data.variable);
            VariableMetaData* var_meta_data = (VariableMetaData*)symbol_table_function_get_or_create(
                                                  optimizer->variables_meta_data,
                                                  var_identifier);
            memory_free(var_identifier);

            if(instruction->type == I_DEF_VAR ||
                    (instruction->type == I_MOVE && i == 0)) // it's first operand
                continue;

            if(instruction->type == I_POP_STACK && instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_END)
                break;

            var_meta_data->occurences_count++;
//            operands[i]->data.variable->meta_data->occurrences_count++;
        }

        instruction = instruction->next;
    }
}

void init_variable_meta_data(SymbolTableBaseItem* item)
{
    VariableMetaData* v = (VariableMetaData*)item;
    v->occurences_count = 0;
}

bool code_optimizer_remove_unused_variables(CodeOptimizer* optimizer)
{
    CodeInstruction* instruction = optimizer->first_instruction;
    const size_t max_operands_count = 3;
    bool remove_something = false;

    // null
//    while(instruction) {
//        const CodeInstructionOperand* operands[] = {
//            instruction->op0, instruction->op1, instruction->op2
//        };

//        for(size_t i = 0; i < max_operands_count; i++) {
//            if(operands[i] == NULL ||
//                    operands[i]->type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
//                continue;
//            operands[i]->data.variable->meta_data->occurrences_count = 0;
//        }

//        instruction = instruction->next;
//    }

    // analyze
    code_optimizer_update_variables_meta_data(optimizer);
//    instruction = optimizer->first_instruction;
//    while(instruction) {
//        const CodeInstructionOperand* operands[] = {
//            instruction->op0, instruction->op1, instruction->op2
//        };

//        for(size_t i = 0; i < max_operands_count; i++) {
//            if(operands[i] == NULL ||
//                    operands[i]->type != TYPE_INSTRUCTION_OPERAND_VARIABLE ||
//                    instruction->type == I_DEF_VAR ||
//                    (instruction->type == I_MOVE && i == 0) // it's first operand
//            )
//                continue;

//            if(instruction->type == I_POP_STACK && instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_END)
//                continue;

//            operands[i]->data.variable->meta_data->occurrences_count++;
//        }

//        instruction = instruction->next;
//    }

    // optimize
    instruction = optimizer->first_instruction;

    while(instruction) {
        bool delete_instruction = false;
        bool delete_expression = false;

        const CodeInstructionOperand* operands[] = {
            instruction->op0, instruction->op1, instruction->op2
        };

        for(size_t i = 0; i < max_operands_count; i++) {
            if(operands[i] == NULL || operands[i]->type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
                continue;

            SymbolVariable* variable = operands[i]->data.variable;
            char* var_identifier = code_instruction_render_variable_identifier(variable);
            const size_t variable_occurrences_count = ((VariableMetaData*)symbol_table_get(optimizer->variables_meta_data, var_identifier))->occurences_count;
            memory_free(var_identifier);

            if(variable_occurrences_count == 0 && variable->frame != VARIABLE_FRAME_TEMP) {
                if(instruction->type == I_POP_STACK && instruction->meta_data.type == CODE_INSTRUCTION_META_TYPE_EXPRESSION_END)
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

    return remove_something;
}
