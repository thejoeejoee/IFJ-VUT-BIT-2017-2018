#include "interpreter.h"
#include "code_instruction_operand.h"

void interpreter_data_stack_push(Interpreter* interpreter, CodeInstructionOperandConstantData data) {
    InterpreterDataStackItem* copied = memory_alloc(sizeof(InterpreterDataStackItem));
    (*copied).data = data;
    stack_push(interpreter->data_stack, (StackBaseItem*) copied);
}

CodeInstructionOperandConstantData interpreter_data_stack_pop(Interpreter* interpreter) {
    StackBaseItem* pop = stack_pop(interpreter->data_stack);
    InterpreterDataStackItem* data = (InterpreterDataStackItem*) pop;
    CodeInstructionOperandConstantData to_return = (*data).data;
    memory_free(data);
    return to_return;
}

void free_operand_value(StackBaseItem* item) {
    InterpreterDataStackItem* data_stack_item = (InterpreterDataStackItem*) item;

    if(data_stack_item->data.data_type == DATA_TYPE_STRING) {
        string_free(&data_stack_item->data.data.string);
    }
}

Interpreter* interpreter_init() {
    Interpreter* interpreter = memory_alloc(sizeof(Interpreter));
    interpreter->data_stack = stack_init(free_operand_value);
    return interpreter;
}

void interpreter_free(Interpreter** interpreter) {
    NULL_POINTER_CHECK(interpreter,);
    NULL_POINTER_CHECK(*interpreter,);

    memory_free(*interpreter);
}

CodeInstructionOperand* interpreter_evaluate_literal_expression(
        Interpreter* interpreter,
        CodeInstruction* start,
        CodeInstruction* end
) {
    NULL_POINTER_CHECK(interpreter, NULL);
    NULL_POINTER_CHECK(start, NULL);
    NULL_POINTER_CHECK(end, NULL);

    CodeInstruction* actual = start;
    CodeInstructionOperandConstantData data;
    do {
        switch(actual->type) {
            DATA_STACK_OPERATION(I_ADD_STACK, +);
            DATA_STACK_OPERATION(I_SUB_STACK, -);
            DATA_STACK_OPERATION(I_MUL_STACK, *);
            DATA_STACK_OPERATION(I_DIV_STACK, /);

            case I_PUSH_STACK: {
                if(actual->op0->type != TYPE_INSTRUCTION_OPERAND_CONSTANT) {
                    LOG_WARNING("Unsupported operand: %d in instruction %d.", actual->op0->type, actual->type);
                    return NULL;
                }
                interpreter_data_stack_push(interpreter, actual->op0->data.constant);
                break;
            }

            default:
                LOG_WARNING("Unsupported instruction %d.", actual->type);
                return NULL;
        }
    } while((actual = actual->next) != end);

    data = interpreter_data_stack_pop(interpreter);
    switch(data.data_type) {
        case DATA_TYPE_INTEGER:
            return code_instruction_operand_init_integer(data.data.integer);
        case DATA_TYPE_DOUBLE:
            return code_instruction_operand_init_double(data.data.double_);
        default:
            LOG_WARNING("Unknown result data type: %d.", data.data_type);
            return NULL;
    }
}
