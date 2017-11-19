#ifndef _INTERPRETER_H
#define _INTERPRETER_H


#include "code_instruction_operand.h"
#include "code_instruction.h"
#include "stack.h"

typedef struct {
    Stack* data_stack;
} Interpreter;

typedef struct {
    StackBaseItem base;

    CodeInstructionOperandConstantData data;
} InterpreterDataStackItem;

Interpreter* interpreter_init();

void interpreter_free(Interpreter** interpreter);

CodeInstructionOperand* interpreter_evaluate_literal_expression(
        Interpreter* interpreter,
        CodeInstruction* start,
        CodeInstruction* end
);

#define DATA_STACK_OPERATION(case_, op) case case_: {\
    CodeInstructionOperandConstantData op1 = interpreter_data_stack_pop(interpreter);\
    CodeInstructionOperandConstantData op2 = interpreter_data_stack_pop(interpreter);\
    ASSERT(op1.data_type == op2.data_type);\
    switch(op1.data_type) {\
        case DATA_TYPE_INTEGER:\
            data.data.integer = op1.data.integer op op2.data.integer;\
            data.data_type = DATA_TYPE_INTEGER;\
            interpreter_data_stack_push(interpreter, data);\
            break;\
        case DATA_TYPE_DOUBLE:\
            data.data.double_ = op1.data.double_ op op2.data.double_;\
            data.data_type = DATA_TYPE_DOUBLE;\
            interpreter_data_stack_push(interpreter, data);\
            break;\
        default:\
            LOG_WARNING("Unsupported data type %d.", op1.data_type);\
            return NULL;\
    }\
}


#endif //_INTERPRETER_H
