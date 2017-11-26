#include "meta_data_peep_hole_pattern.h"

void init_mapped_operand_item(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    MappedOperand* v = (MappedOperand*) item;
    v->operand = NULL;
}

void free_mapped_operand_item(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    MappedOperand* v = (MappedOperand*) item;
    if(v->operand != NULL)
        code_instruction_operand_free(&v->operand);
}

void init_peep_hole_pattern(LListBaseItem* item) {
    PeepHolePattern* v = (PeepHolePattern*) item;
    llist_init(&v->matching_instructions, sizeof(PeepHolePatternInstruction), NULL, NULL, NULL);
    llist_init(&v->replacement_instructions, sizeof(PeepHolePatternInstruction), NULL, NULL, NULL);
}

void free_peep_hole_pattern(LListBaseItem* item) {
    PeepHolePattern* v = (PeepHolePattern*) item;
    llist_free(&v->matching_instructions);
    llist_free(&v->replacement_instructions);
}

void code_optimizer_add_matching_instruction_to_ph_pattern(PeepHolePattern* ph_pattern, TypeInstruction instruction,
                                                           const char* op1_alias, const char* op2_alias,
                                                           const char* op3_alias, int op0_occ_count, int op1_occ_count,
                                                           int op2_occ_count) {
    _code_optimizer_add_instruction_to_ph_pattern(ph_pattern->matching_instructions, instruction, op1_alias, op2_alias,
                                                  op3_alias, op0_occ_count, op1_occ_count, op2_occ_count);
}

void code_optimizer_add_replacement_instruction_to_ph_pattern(PeepHolePattern* ph_pattern, TypeInstruction instruction,
                                                              const char* op1_alias, const char* op2_alias,
                                                              const char* op3_alias) {
    _code_optimizer_add_instruction_to_ph_pattern(ph_pattern->replacement_instructions, instruction, op1_alias,
                                                  op2_alias, op3_alias, 0, 0, 0);
}

void _code_optimizer_add_instruction_to_ph_pattern(LList* pattern_instruction_sub_list, TypeInstruction instruction,
                                                   const char* op1_alias, const char* op2_alias, const char* op3_alias,
                                                   int op0_occ_count, int op1_occ_count, int op2_occ_count) {
    NULL_POINTER_CHECK(pattern_instruction_sub_list,);

    PeepHolePatternInstruction* ph_pattern_instruction = (PeepHolePatternInstruction*) llist_new_tail_item(
            pattern_instruction_sub_list
    );
    ph_pattern_instruction->type = instruction;
    ph_pattern_instruction->op0_alias = op1_alias;
    ph_pattern_instruction->op1_alias = op2_alias;
    ph_pattern_instruction->op2_alias = op3_alias;

    ph_pattern_instruction->op0_occurrences_count = op0_occ_count;
    ph_pattern_instruction->op1_occurrences_count = op1_occ_count;
    ph_pattern_instruction->op2_occurrences_count = op2_occ_count;
}


MetaPHPatternFlag extract_flag(const char* alias) {
    if(alias == NULL || strlen(alias) == 0)
        return META_PATTERN_FLAG_INVALID;

    switch(alias[0]) {
        case ']':
            return META_PATTERN_FLAG_STRING_EMPTY;
        case '[':
            return META_PATTERN_FLAG_STRING;
        case '!':
            return META_PATTERN_FLAG_VARIABLE;
        case '}':
            return META_PATTERN_FLAG_INT_LITERAL_ZERO;
        case '{':
            return META_PATTERN_FLAG_INT_LITERAL;
        case ')':
            return META_PATTERN_FLAG_FLOAT_LITERAL_ZERO;
        case '(':
            return META_PATTERN_FLAG_FLOAT_LITERAL;
        case '|':
            return META_PATTERN_FLAG_BOOL_LITERAL;
        case '<':
            return META_PATTERN_FLAG_BOOL_LITERAL_TRUE;
        case '>':
            return META_PATTERN_FLAG_BOOL_LITERAL_FALSE;
        case '&':
            return META_PATTERN_FLAG_LABEL;
        case '1':
            return META_PATTERN_FLAG_TEMP_VARIABLE_1;
        case '2':
            return META_PATTERN_FLAG_TEMP_VARIABLE_2;
        case '3':
            return META_PATTERN_FLAG_TEMP_VARIABLE_3;
        case '4':
            return META_PATTERN_FLAG_TEMP_VARIABLE_4;
        case '5':
            return META_PATTERN_FLAG_TEMP_VARIABLE_5;
        default:
            return META_PATTERN_FLAG_ALL;
    }
}
