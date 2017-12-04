#include "code_optimizer_expr.h"
#include "code_instruction_operand.h"

bool is_rounded_zero_integer(ExprToken* token) {
    NULL_POINTER_CHECK(token, true);
    if(token->instruction == NULL) {
        return true;
    }
    if(token->instruction->op0 == NULL) {
        return true;
    }
    if(token->instruction->op0->type != TYPE_INSTRUCTION_OPERAND_CONSTANT) {
        return true;
    }
    if(token->instruction->op0->data.constant.data_type == DATA_TYPE_INTEGER) {
        return token->instruction->op0->data.constant.data.integer == 0;
    } else if(token->instruction->op0->data.constant.data_type == DATA_TYPE_DOUBLE) {
        return round_even(token->instruction->op0->data.constant.data.double_) == 0;
    }

    return true;

}

CodeInstructionOperand* code_optimizer_expr_eval(
        CodeOptimizer* optimizer,
        ExprToken* t1, ExprToken* t2, ExprToken* result,
        const OperationSignature* signature
) {
    NULL_POINTER_CHECK(optimizer, NULL);
    NULL_POINTER_CHECK(signature, NULL);
    NULL_POINTER_CHECK(t1, NULL);
    NULL_POINTER_CHECK(result, NULL);

    if(!t1->is_constant)
        return NULL;
    if((t2 != NULL && !t2->is_constant))
        return NULL;

    // expecting correct data types - by implicit conversions czech check

    int result_i = 0;
    double result_d = 0;
    String* result_s = string_init();
    // sorry, but too many cases here
    memory_free_lazy(result_s);
    memory_free_lazy(result_s->content);

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
                        return NULL;
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
                        return NULL;
                }
                break;
            }
            default:
                LOG_WARNING("Unknown unary operation %d.", signature->operation_type);
                return NULL;
        }
    } else {
        switch(signature->operation_type) {
            case OPERATION_ADD: {
                switch(signature->result_type) {
                    case DATA_TYPE_DOUBLE:
                        CEE_ENABLED_CHECK();
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_DOUBLE, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_DOUBLE, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_d, +);
                        break;

                    case DATA_TYPE_INTEGER:
                        CEE_ENABLED_CHECK();
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_i, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_i, +);
                        break;

                    case DATA_TYPE_STRING:
                        if(t1->data_type == DATA_TYPE_STRING)
                            string_append(result_s, t1->instruction->op0->data.constant.data.string);
                        else {
                            LOG_WARNING("Unknown data type");
                            return NULL;
                        }

                        if(t2->data_type == DATA_TYPE_STRING)
                            string_append(result_s, t2->instruction->op0->data.constant.data.string);
                        else {
                            LOG_WARNING("Unknown data type");
                            return NULL;
                        }

                        break;
                    default:
                        LOG_WARNING("Unknown operation");
                        return NULL;
                }
                break;
            }
            case OPERATION_SUB: {
                CEE_ENABLED_CHECK();
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
                        return NULL;
                }
                break;
            }
            case OPERATION_MULTIPLY: {
                CEE_ENABLED_CHECK();
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
                        return NULL;
                }
                break;
            }
            case OPERATION_DIVIDE: {
                CEE_ENABLED_CHECK();
                if(is_rounded_zero_integer(t2))
                    return NULL;
                switch(signature->result_type) {
                    case DATA_TYPE_DOUBLE:
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_DOUBLE, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_d, +);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_DOUBLE, result_d, /);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_d, /);
                        break;

                    default:
                        LOG_WARNING("Unknown operation");
                        return NULL;
                }
                break;
            }

            case OPERATION_INT_DIVIDE: {
                CEE_ENABLED_CHECK();
                if(is_rounded_zero_integer(t2))
                    return NULL;
                switch(signature->result_type) {
                    case DATA_TYPE_INTEGER:
                        if(t1->data_type == DATA_TYPE_DOUBLE)
                            result_i = round_even(t1->instruction->op0->data.constant.data.double_);
                        TRY_TO_PERFORM_OPERATION(t1, DATA_TYPE_INTEGER, result_i, +);

                        if(t2->data_type == DATA_TYPE_DOUBLE)
                            result_i /= round_even(t2->instruction->op0->data.constant.data.double_);
                        TRY_TO_PERFORM_OPERATION(t2, DATA_TYPE_INTEGER, result_i, /);
                        break;

                    default:
                        LOG_WARNING("Unknown operation");
                        return NULL;
                }
                break;
            }
            case OPERATION_OR:
            case OPERATION_AND: {
                if(t1->data_type == DATA_TYPE_BOOLEAN && t2->data_type == DATA_TYPE_BOOLEAN) {
                    if(signature->operation_type == OPERATION_AND)
                        result_i = t1->instruction->op0->data.constant.data.boolean &&
                                   t2->instruction->op0->data.constant.data.boolean;
                    else
                        result_i = t1->instruction->op0->data.constant.data.boolean ||
                                   t2->instruction->op0->data.constant.data.boolean;
                } else {
                    LOG_WARNING("Unsupported operand types to 'and' or 'or' operation.");
                    return NULL;
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
                        return NULL;
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
                        return NULL;
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
                        return NULL;
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
                        return NULL;
                }
                break;
            }

            case OPERATION_NOT_EQUAL:
            case OPERATION_EQUAL: {
                switch(signature->result_type) {
                    case DATA_TYPE_BOOLEAN:
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_INTEGER, result_i, ==);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE, result_i, ==);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_INTEGER, result_i, ==);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE, result_i, ==);
                        TRY_TO_PERFORM_BINARY_OPERATION(t1, t2, DATA_TYPE_BOOLEAN, DATA_TYPE_BOOLEAN, result_i, ==);
                        const DataType t1_type = t1->instruction->op0->data.constant.data_type;
                        const DataType t2_type = t2->instruction->op0->data.constant.data_type;

                        if(t1_type == DATA_TYPE_STRING && t2_type == DATA_TYPE_STRING) {
                            result_i = 0 == strcmp(
                                    t1->instruction->op0->data.constant.data.string->content,
                                    t2->instruction->op0->data.constant.data.string->content
                            );
                        }
                        if(signature->operation_type == OPERATION_NOT_EQUAL)
                            result_i = !result_i;
                        break;
                    default:
                        LOG_WARNING("Unknown data type of operands.");
                        return NULL;
                }
                break;
            }

            default:
                LOG_WARNING("Unknown binary operation %d.", signature->operation_type);
                return NULL;
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

int round_even(double x) {
    x -= remainder(x, 1.0);
    return (int) x;
}

void code_optimizer_optimize_type_casts(CodeOptimizer* optimizer) {
    NULL_POINTER_CHECK(optimizer,);
    CodeGenerator* generator = optimizer->generator;
    CodeInstruction* start = generator->first;
    CodeInstruction* end = generator->last;
    NULL_POINTER_CHECK(start,);
    NULL_POINTER_CHECK(end,);

    CodeInstruction* actual = start;
    CodeInstruction* next;
    CodeInstruction* replacement;
    do {
        next = actual->next;
        switch(actual->type) {
            case I_FLOAT_ROUND_TO_EVEN_INT_STACK: {
                if(actual->prev != NULL &&
                   actual->prev->type == I_PUSH_STACK &&
                   actual->prev->op0->type == TYPE_INSTRUCTION_OPERAND_CONSTANT) {
                    if(actual->prev->op0->data.constant.data_type != DATA_TYPE_DOUBLE) {
                        LOG_WARNING("Invalid operand to type cast.");
                        continue;
                    }

                    replacement = code_generator_new_instruction(
                            generator,
                            I_PUSH_STACK,
                            code_instruction_operand_init_integer(
                                    round_even(actual->prev->op0->data.constant.data.double_)
                            ),
                            NULL,
                            NULL
                    );
                    code_generator_insert_instruction_before(
                            generator,
                            replacement,
                            actual->prev
                    );
                    code_generator_remove_instruction(generator, actual->prev);
                    code_generator_remove_instruction(generator, actual);
                    next = replacement;
                }
            }
                break;
            case I_INT_TO_FLOAT_STACK: {
                if(actual->prev != NULL &&
                   actual->prev->type == I_PUSH_STACK &&
                   actual->prev->op0->type == TYPE_INSTRUCTION_OPERAND_CONSTANT) {
                    if(actual->prev->op0->data.constant.data_type != DATA_TYPE_INTEGER) {
                        LOG_WARNING("Invalid operand to type cast.");
                        continue;
                    }

                    replacement = code_generator_new_instruction(
                            generator,
                            I_PUSH_STACK,
                            code_instruction_operand_init_double(
                                    actual->prev->op0->data.constant.data.integer
                            ),
                            NULL,
                            NULL
                    );
                    replacement->meta_data = actual->prev->meta_data;

                    code_generator_insert_instruction_before(
                            generator,
                            replacement,
                            actual->prev
                    );
                    code_generator_remove_instruction(generator, actual->prev);
                    code_generator_remove_instruction(generator, actual);
                    next = replacement;
                }
            }
            default:;
        }
        actual = next;
    } while(actual != end);
}
