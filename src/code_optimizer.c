#include "code_optimizer.h"
#include "memory.h"

CodeOptimizer* code_optimizer_init(CodeGenerator* generator)
{
    NULL_POINTER_CHECK(generator, NULL);

    CodeOptimizer* optimizer = memory_alloc(sizeof(CodeOptimizer));
    optimizer->generator = generator;
    optimizer->first_instruction = generator->first;

    return optimizer;
}

void code_optimizer_free(CodeOptimizer** optimizer)
{
    memory_free(*optimizer);
    *optimizer = NULL;
}

void init_variable_meta_data(SymbolTableBaseItem* item)
{
    VariableMetaData* v = (VariableMetaData*)item;
    v->occurences_count = 0;
}

void code_optimizer_remove_unused_variables(CodeOptimizer* optimizer)
{
    SymbolTable* variables_table = symbol_table_init(32, sizeof(VariableMetaData), &init_variable_meta_data, NULL);

    CodeInstruction* instruction = optimizer->first_instruction;
    const size_t max_operands_count = 3;
    char* rendered_identifier;

    // analyze
    while(instruction) {
        const CodeInstructionOperand* operands[] = {
            instruction->op0, instruction->op1, instruction->op2
        };

        for(size_t i = 0; i < max_operands_count; i++) {
            if(operands[i] == NULL ||
                    operands[i]->type != TYPE_INSTRUCTION_OPERAND_VARIABLE ||
                    instruction->type == I_DEF_VAR ||
                    (instruction->type == I_MOVE && i == 0) // it's first operand
            )
                continue;


            const size_t identifier_max_len = code_instruction_rendered_variable_identifier_max_len(operands[i]->data.variable);

            rendered_identifier = memory_alloc(sizeof(char) * identifier_max_len);
            code_instruction_operand_render_variable_identifier(operands[i]->data.variable, rendered_identifier, identifier_max_len);

            VariableMetaData* variable_meta_data = (VariableMetaData*)symbol_table_get_or_create(variables_table, rendered_identifier);

            variable_meta_data->occurences_count++;
            memory_free(rendered_identifier);
        }

        instruction = instruction->next;
    }

    // optimize
    instruction = optimizer->first_instruction;
    bool delete_instruction = false;

    while(instruction) {
        delete_instruction = false;
        const CodeInstructionOperand* operands[] = {
            instruction->op0, instruction->op1, instruction->op2
        };

        for(size_t i = 0; i < max_operands_count; i++) {
            if(operands[i] == NULL || operands[i]->type != TYPE_INSTRUCTION_OPERAND_VARIABLE)
                continue;

            const size_t identifier_max_len = code_instruction_rendered_variable_identifier_max_len(operands[i]->data.variable);

            rendered_identifier = memory_alloc(sizeof(char) * identifier_max_len);
            code_instruction_operand_render_variable_identifier(operands[i]->data.variable, rendered_identifier, identifier_max_len);

            VariableMetaData* variable_meta_data = (VariableMetaData*)symbol_table_get_or_create(variables_table, rendered_identifier);

            if(variable_meta_data->occurences_count == 0) {
                delete_instruction = true;
                memory_free(rendered_identifier);
                break;
            }


            memory_free(rendered_identifier);
        }

        if(delete_instruction) {
            CodeInstruction* temp = instruction->next;
            code_generator_remove_instruction(optimizer->generator, instruction);
            instruction = temp;
        }

        else
            instruction = instruction->next;
    }

    symbol_table_free(variables_table);
}
