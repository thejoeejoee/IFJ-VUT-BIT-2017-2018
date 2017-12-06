#include <stdbool.h>
#include <math.h>
#include "code_instruction_operand.h"
#include "parser_expr_internal.h"
#include "parser_semantic.h"
#include "code_constructor.h"
#include "code_optimizer.h"

#ifndef _CODE_OPTIMIZER_EXPR_H
#define _CODE_OPTIMIZER_EXPR_H

#ifndef CEE_ENABLED
#define CEE_ENABLED_CHECK() do { } while(0)
#define CEE_ENABLED true
#else
#define CEE_ENABLED_CHECK() do { return NULL; } while(0)
#define CEE_ENABLED false
#endif

#define TRY_TO_PERFORM_BINARY_OPERATION(token_1, token_2, data_type_1, data_type_2, result, op) do {\
    if((token_1)->data_type == (data_type_1) && (token_2)->data_type == (data_type_2)) {\
        if ((data_type_1) == DATA_TYPE_INTEGER && (data_type_2) == DATA_TYPE_INTEGER)\
            (result) = (token_1)->instruction->op0->data.constant.data.integer op (token_2)->instruction->op0->data.constant.data.integer; \
        \
        else if ((data_type_1) == DATA_TYPE_INTEGER && (data_type_2) == DATA_TYPE_DOUBLE)\
            (result) = (token_1)->instruction->op0->data.constant.data.integer op (token_2)->instruction->op0->data.constant.data.double_; \
        \
        else if ((data_type_1) == DATA_TYPE_DOUBLE && (data_type_2) == DATA_TYPE_INTEGER)\
            (result) = (token_1)->instruction->op0->data.constant.data.double_ op (token_2)->instruction->op0->data.constant.data.integer; \
        \
        else if ((data_type_1) == DATA_TYPE_INTEGER && (data_type_2) == DATA_TYPE_BOOLEAN)\
            (result) = (token_1)->instruction->op0->data.constant.data.integer op (token_2)->instruction->op0->data.constant.data.boolean; \
        \
        else if ((data_type_1) == DATA_TYPE_BOOLEAN && (data_type_2) == DATA_TYPE_INTEGER)\
            (result) = (token_1)->instruction->op0->data.constant.data.boolean op (token_2)->instruction->op0->data.constant.data.integer; \
        \
        else if ((data_type_1) == DATA_TYPE_DOUBLE && (data_type_2) == DATA_TYPE_DOUBLE)\
            (result) = (token_1)->instruction->op0->data.constant.data.double_ op (token_2)->instruction->op0->data.constant.data.double_; \
        \
        else if ((data_type_1) == DATA_TYPE_DOUBLE && (data_type_2) == DATA_TYPE_BOOLEAN)\
            (result) = (token_1)->instruction->op0->data.constant.data.double_ op (token_2)->instruction->op0->data.constant.data.boolean; \
        \
        else if ((data_type_1) == DATA_TYPE_BOOLEAN && (data_type_2) == DATA_TYPE_DOUBLE)\
            (result) = (token_1)->instruction->op0->data.constant.data.boolean op (token_2)->instruction->op0->data.constant.data.double_; \
        \
        else if ((data_type_1) == DATA_TYPE_BOOLEAN && (data_type_2) == DATA_TYPE_BOOLEAN)\
            (result) = (token_1)->instruction->op0->data.constant.data.boolean op (token_2)->instruction->op0->data.constant.data.boolean; \
}} while(0)

#define TRY_TO_PERFORM_OPERATION(token, data_type_, result, op) do { \
    if((token)->data_type == (data_type_)) {\
        if ((data_type_) == DATA_TYPE_INTEGER)\
            (result) = (result) op (token)->instruction->op0->data.constant.data.integer; \
        else if ((data_type_) == DATA_TYPE_DOUBLE)\
            (result) = (result) op (token)->instruction->op0->data.constant.data.double_; \
        else if ((data_type_) == DATA_TYPE_BOOLEAN)\
            (result) = (result) op (token)->instruction->op0->data.constant.data.boolean; \
        else { LOG_WARNING("Unknown data type"); } \
}} while(0)

/**
 * Try to 'constantize' operation `E op E` by defined signature, source T1 a T2 and result E.
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

/**
 * Try replace PUSH lit, CONVERSION_ON_STACK by PUSH conversed_literal.
 * @param optimizer instance
 */
void code_optimizer_optimize_type_casts(CodeOptimizer* optimizer);

/**
 * Try to replace all PUSH lit, PUSH lit, OP_STACK by PUSH (lit op lit).
 * @param optimizer instance
 */
void code_optimizer_optimize_partial_expression_eval(CodeOptimizer* optimizer);


void code_optimizer_optimize_jumps(CodeOptimizer* optimizer);

void code_optimizer_optimize_comparisons(CodeOptimizer* optimizer);

void code_optimizer_multi_write(CodeOptimizer* optimizer);

#endif //_CODE_OPTIMIZER_EXPR_H
