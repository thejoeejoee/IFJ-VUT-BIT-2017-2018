#include "interpreter.h"
#include "code_instruction_operand.h"
#include "code_optimizer_expr.h"

void interpreter_data_stack_push(Interpreter* interpreter, CodeInstructionOperandConstantData data) {
    InterpreterDataStackItem* copied = memory_alloc(sizeof(InterpreterDataStackItem));
    (*copied).data = data;
    stack_push(interpreter->data_stack, (StackBaseItem*) copied);
}

CodeInstructionOperandConstantData interpreter_data_stack_pop(Interpreter* interpreter) {
    CodeInstructionOperandConstantData to_return = {.data_type=DATA_TYPE_NONE};
    StackBaseItem* pop = stack_pop(interpreter->data_stack);
    NULL_POINTER_CHECK(pop, to_return);
    InterpreterDataStackItem* data = (InterpreterDataStackItem*) pop;
    to_return = (*data).data;
    memory_free(data);
    return to_return;
}

void free_operand_value(StackBaseItem* item) {
    InterpreterDataStackItem* data_stack_item = (InterpreterDataStackItem*) item;

    if(data_stack_item->data.data_type == DATA_TYPE_STRING) {
        string_free(&data_stack_item->data.data.string);
    }
}

Interpreter* interpreter_init(SymbolVariable* cast_buffer) {
    Interpreter* interpreter = memory_alloc(sizeof(Interpreter));
    interpreter->data_stack = stack_init(free_operand_value);
    interpreter->buffer_used = false;
    interpreter->cast_buffer = cast_buffer;
    return interpreter;
}

void interpreter_free(Interpreter** interpreter) {
    NULL_POINTER_CHECK(interpreter,);
    NULL_POINTER_CHECK(*interpreter,);

    stack_free(&(*interpreter)->data_stack);
    memory_free(*interpreter);
}

CodeInstructionOperand* interpreter_evaluate_instruction_block(
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
            DATA_STACK_OPERATION(I_SUB_STACK, -);
            DATA_STACK_OPERATION(I_ADD_STACK, +);
            DATA_STACK_OPERATION(I_MUL_STACK, *);
            DATA_STACK_OPERATION(I_DIV_STACK, /);
            DATA_STACK_OPERATION(I_AND_STACK, &&);
            DATA_STACK_OPERATION(I_OR_STACK, ||);
            DATA_STACK_CMP_OPERATION(I_EQUAL_STACK, ==);
            DATA_STACK_CMP_OPERATION(I_GREATER_THEN_STACK, >);
            DATA_STACK_CMP_OPERATION(I_LESSER_THEN_STACK, <);

            case I_NOT_STACK: {
                CodeInstructionOperandConstantData to_not = interpreter_data_stack_pop(interpreter);
                if(to_not.data_type != DATA_TYPE_BOOLEAN) {
                    LOG_WARNING("Invalid data type to not: %d", to_not.data_type);
                    return NULL;
                }
                to_not.data.boolean = !to_not.data.boolean;
                interpreter_data_stack_push(interpreter, to_not);
                break;
            }

            case I_INT_TO_FLOAT_STACK: {
                CodeInstructionOperandConstantData to_cast = interpreter_data_stack_pop(interpreter);
                if(to_cast.data_type != DATA_TYPE_INTEGER) {
                    LOG_WARNING("Invalid data type to convert: %d", to_cast.data_type);
                    return NULL;
                }
                CodeInstructionOperandConstantData casted = {
                        .data_type = DATA_TYPE_DOUBLE,
                        .data = {
                                .double_=to_cast.data.integer
                        }
                };
                interpreter_data_stack_push(interpreter, casted);
                break;
            }
            case I_FLOAT_ROUND_TO_EVEN_INT_STACK: {
                CodeInstructionOperandConstantData to_cast = interpreter_data_stack_pop(interpreter);
                if(to_cast.data_type != DATA_TYPE_DOUBLE) {
                    LOG_WARNING("Invalid data type to convert: %d", to_cast.data_type);
                    return NULL;
                }
                CodeInstructionOperandConstantData casted = {
                        .data_type = DATA_TYPE_INTEGER,
                        .data = {
                                .integer=round_even(to_cast.data.double_)
                        }
                };
                interpreter_data_stack_push(interpreter, casted);
                break;
            }

            case I_PUSH_STACK: {
                if(actual->op0->type == TYPE_INSTRUCTION_OPERAND_VARIABLE && symbol_variable_cmp(
                        actual->op0->data.variable,
                        interpreter->cast_buffer
                )) {
                    if(!interpreter->buffer_used) {
                        LOG_WARNING("Buffer underflow.");
                        return NULL;
                    }
                    interpreter_data_stack_push(interpreter, interpreter->buffer);
                    interpreter->buffer_used = false;
                    break;
                }

                if(actual->op0->type != TYPE_INSTRUCTION_OPERAND_CONSTANT) {
                    LOG_WARNING("Unsupported operand: %d in instruction %d.", actual->op0->type, actual->type);
                    return NULL;
                }
                interpreter_data_stack_push(interpreter, actual->op0->data.constant);
                break;
            }

            case I_POP_STACK: {
                if(actual->op0->type != TYPE_INSTRUCTION_OPERAND_VARIABLE || symbol_variable_cmp(
                        actual->op0->data.variable,
                        interpreter->cast_buffer
                )) {
                    // is buffer
                    if(interpreter->buffer_used) {
                        LOG_WARNING("Buffer overflow.");
                        return NULL;
                    }
                    interpreter->buffer = interpreter_data_stack_pop(interpreter);
                    interpreter->buffer_used = true;
                    break;
                }
                LOG_WARNING("Unsupported pop from stack.");
                break;
            }

            default:
                LOG_WARNING("Unsupported instruction %d.", actual->type);
                return NULL;
        }
    } while((actual = actual->next) != end->next);

    data = interpreter_data_stack_pop(interpreter);
    switch(data.data_type) {
        case DATA_TYPE_INTEGER:
            return code_instruction_operand_init_integer(data.data.integer);
        case DATA_TYPE_DOUBLE:
            return code_instruction_operand_init_double(data.data.double_);
        case DATA_TYPE_BOOLEAN:
            return code_instruction_operand_init_boolean(data.data.boolean);
        default:
            LOG_WARNING("Unknown result data type: %d.", data.data_type);
            return NULL;
    }
}
