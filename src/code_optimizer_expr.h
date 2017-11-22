#include <stdbool.h>
#include <math.h>
#include "code_instruction_operand.h"
#include "parser_expr_internal.h"
#include "parser_semantic.h"
#include "code_constructor.h"
#include "code_optimizer.h"

#ifndef _CODE_OPTIMIZER_EXPR_H
#define _CODE_OPTIMIZER_EXPR_H

#define TRY_TO_PERFORM_OPERATION(token, data_type_, result, op) do { \
    if((token)->data_type == (data_type_)) {\
        NULL_POINTER_CHECK((token)->instruction, NULL); \
        NULL_POINTER_CHECK((token)->instruction->op0, NULL); \
        if ((data_type_) == DATA_TYPE_INTEGER)\
            (result) = (result) op (token)->instruction->op0->data.constant.data.integer; \
        else if ((data_type_) == DATA_TYPE_DOUBLE)\
            (result) = (result) op (token)->instruction->op0->data.constant.data.double_; \
        else if ((data_type_) == DATA_TYPE_BOOLEAN)\
            (result) = (result) op (token)->instruction->op0->data.constant.data.boolean; \
        else LOG_WARNING("Unknown data type"); \
}} while(0)

#define OPERAND_VALUE(operand) \
    (operand)->data.constant.data_type == DATA_TYPE_INTEGER ? (operand)->data.constant.data.integer

/**
 * Try 'constantize' operation `E op E` by defined signature, source T1 a T2 and result E.
 * @param optimizer instance
 * @param t1 first expr token
 * @param t2 second expr token - for unary ops is NULL
 * @param result result expr token to keep constant flag
 * @param signature signature of operation
 * @return operand to replace
 */
CodeInstructionOperand* code_optimizer_expr_eval(
        CodeOptimizer* optimizer,
        ExprToken* t1, ExprToken* t2, ExprToken* result, const OperationSignature* signature
);


#endif //_CODE_OPTIMIZER_EXPR_H
