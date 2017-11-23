#include "code_optimizer_expr.h"
#include "code_instruction_operand.h"


CodeInstructionOperand* code_optimizer_expr_eval(
        CodeOptimizer* optimizer,
        ExprToken* t1, ExprToken* t2, ExprToken* result,
        const OperationSignature* signature
) {
    NULL_POINTER_CHECK(optimizer, NULL);
    NULL_POINTER_CHECK(signature, NULL);
    NULL_POINTER_CHECK(t1, NULL);
    NULL_POINTER_CHECK(result, NULL);
    // temporary disabled
    return NULL;

    if(!t1->is_constant)
        return NULL;
    if((t2 != NULL && !t2->is_constant))
        return NULL;

    // expecting correct data types - by implicit conversions czech check

    int result_i = 0;
    double result_d = 0;
    String* result_s = string_init();

    if(t2 == NULL) {
        // unary op
        switch(signature->operation_type) {
            case OPERATION_UNARY_MINUS: {
                switch(t1->data_type) {
                    case DATA_TYPE_INTEGER:
                        result_i = -t1->instruction->op0->data.constant.data.integer;
                        break;
                    case DATA_TYPE_DOUBLE:
                        result_d = -t1->instruction->op0->data.constant.data.double_;
                        break;
                    default:
                        LOG_WARNING("Unknown data type to process unary minus.");
                }
                break;
            }
            case OPERATION_NOT: {
                switch(t1->data_type) {
                    case DATA_TYPE_BOOLEAN:
                        result_i = !t1->instruction->op0->data.constant.data.boolean;
                        break;
                    default:
                        LOG_WARNING("Unknown data type to process not.");
                }
                break;
            }
            default:
                LOG_WARNING("Unknown unary operation %d.", signature->operation_type);
        }
    } else {
        switch(signature->operation_type) {
            case OPERATION_ADD: {
                switch(signature->result_type) {
                    case DATA_TYPE_DOUBLE:
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_DOUBLE, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_DOUBLE, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_d, +);
                        break;

                    case DATA_TYPE_INTEGER:
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_i, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_i, +);
                        break;

                    case DATA_TYPE_STRING:
                        if(t1->data_type == DATA_TYPE_STRING)
                            string_append(result_s, t1->instruction->op0->data.constant.data.string);
                        else {
                            LOG_WARNING("Unknown data type");
                        }

                        if(t2->data_type == DATA_TYPE_STRING)
                            string_append(result_s, t2->instruction->op0->data.constant.data.string);
                        else {
                            LOG_WARNING("Unknown data type");
                        }

                        break;
                    default:
                        LOG_WARNING("Unknown operation");
                }
                break;
            }
            case OPERATION_SUB: {
                switch(signature->result_type) {
                    case DATA_TYPE_DOUBLE:
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_DOUBLE, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_DOUBLE, result_d, -);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_d, -);
                        break;

                    case DATA_TYPE_INTEGER:
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_i, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_i, -);
                        break;
                    default:
                        LOG_WARNING("Unknown operation");
                }
                break;
            }
            case OPERATION_MULTIPLY: {
                switch(signature->result_type) {
                    case DATA_TYPE_DOUBLE:
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_DOUBLE, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_DOUBLE, result_d, *);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_d, *);
                        break;

                    case DATA_TYPE_INTEGER:
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_i, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_i, *);
                        break;
                    default:
                        LOG_WARNING("Unknown operation");
                }
                break;
            }
            case OPERATION_DIVIDE:
                // return NULL;
            case OPERATION_INT_DIVIDE: {
                switch(signature->result_type) {
                    case DATA_TYPE_DOUBLE:
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_DOUBLE, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_DOUBLE, result_d, /);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_d, /);
                        break;

                    case DATA_TYPE_INTEGER:
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_DOUBLE, result_i, +);
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_i, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_DOUBLE, result_i, /);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_i, /);
                        break;

                    default:
                        LOG_WARNING("Unknown operation");
                }
                break;
            }

            case OPERATION_GREATER: {
                switch(signature->result_type) {
                    case DATA_TYPE_BOOLEAN:
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_INTEGER, result_i, >);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE, result_i, >);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_INTEGER, result_i, >);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE, result_i, >);
                        break;
                    default:
                        LOG_WARNING("Unknown operation");
                }
                break;
            }

            case OPERATION_GREATER_OR_EQUAL: {
                switch(signature->result_type) {
                    case DATA_TYPE_BOOLEAN:
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_INTEGER, result_i, >=);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE, result_i, >=);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_INTEGER, result_i, >=);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE, result_i, >=);
                        break;
                    default:
                        LOG_WARNING("Unknown operation");
                }
                break;
            }

            case OPERATION_LESSER: {
                switch(signature->result_type) {
                    case DATA_TYPE_BOOLEAN:
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_INTEGER, result_i, <);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE, result_i, <);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_INTEGER, result_i, <);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE, result_i, <);
                        break;
                    default:
                        LOG_WARNING("Unknown operation");
                }
                break;
            }

            case OPERATION_LESSER_OR_EQUAL: {
                switch(signature->result_type) {
                    case DATA_TYPE_BOOLEAN:
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_INTEGER, result_i, <=);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE, result_i, <=);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_INTEGER, result_i, <=);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE, result_i, <=);
                        break;
                    default:
                        LOG_WARNING("Unknown operation");
                }
                break;
            }
            default:
                LOG_WARNING("Unknown binary operation %d.", signature->operation_type);
        }
    }

    result->is_constant = true;
    result->data_type = signature->result_type;
    switch(signature->result_type) {
        case DATA_TYPE_INTEGER:
            return code_instruction_operand_init_integer(result_i);
        case DATA_TYPE_DOUBLE:
            return code_instruction_operand_init_double(result_d);
        case DATA_TYPE_STRING:
            return code_instruction_operand_init_string(result_s);
        case DATA_TYPE_BOOLEAN:
            return code_instruction_operand_init_boolean((bool) result_i);
        default:
            LOG_WARNING("Unknown result type of given signature.");
    }
    return NULL;
}

