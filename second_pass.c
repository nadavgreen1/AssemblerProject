#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "second_pass.h"
#include "globals.h"
#include "symbol_table.h"
#include "error_handler.h"
#include "lexer.h"
#include "util.h"
#include "memory_image.h"
#include "instructions.h"

int count_extra_words(int src_mode, int dst_mode, int num_operands) {
    if (num_operands == 0) return 0;

    if (num_operands == 1) {
        return (dst_mode == ADDR_MATRIX) ? 3 : 1;
    }

    if (num_operands == 2) {
        if (src_mode == ADDR_REGISTER && dst_mode == ADDR_REGISTER)
            return 1;

        int count = 0;
        count += (src_mode == ADDR_MATRIX) ? 3 : 1;
        count += (dst_mode == ADDR_MATRIX) ? 3 : 1;

        return count;
    }

    return 0;
}

void second_pass(FILE *source_file) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int last_encoded_ic = -1;
    int last_line_encoded = -1;

    IC = MEMORY_START;
    rewind(source_file);

    while (fgets(line, MAX_LINE_LENGTH, source_file)) {
        const char *cursor = line;
        char token[MAX_LABEL_LENGTH];
        char operand1[MAX_LABEL_LENGTH] = "";
        char operand2[MAX_LABEL_LENGTH] = "";

        line_number++;
        cursor = skip_whitespace(cursor);
        if (is_comment_or_empty(cursor)) continue;

        extract_token(token, cursor, MAX_LABEL_LENGTH);

        if (token[strlen(token) - 1] == ':') {
            cursor += strlen(token);
            cursor = skip_whitespace(cursor);
            extract_token(token, cursor, MAX_LABEL_LENGTH);
        }

        if (is_macro_call(token)) continue;

        if (strcmp(token, ".entry") == 0) {
            cursor += strlen(token);
            cursor = skip_whitespace(cursor);
            extract_token(token, cursor, MAX_LABEL_LENGTH);
            mark_entry_symbol(token);
            continue;
        }

        if (strcmp(token, ".extern") == 0 || strcmp(token, ".data") == 0 ||
            strcmp(token, ".string") == 0 || strcmp(token, ".mat") == 0)
            continue;

        InstructionInfo *info = get_instruction_info(token);
        if (!info) {
            report_error("Unknown instruction", line_number);
            continue;
        }

        cursor += strlen(token);
        cursor = skip_whitespace(cursor);

        if (info->num_operands >= 1) {
            extract_token(operand1, cursor, MAX_LABEL_LENGTH);
            cursor += strlen(operand1);
            cursor = skip_whitespace(cursor);
        }

        if (info->num_operands == 2) {
            if (*cursor != ',') {
                report_error("Missing comma between operands", line_number);
                continue;
            }
            cursor++;
            cursor = skip_whitespace(cursor);
            extract_token(operand2, cursor, MAX_LABEL_LENGTH);
            cursor += strlen(operand2);
            cursor = skip_whitespace(cursor);
        }

        if (!is_comment_or_empty(cursor)) {
            report_error("Extra characters after instruction", line_number);
            continue;
        }

        int src_mode = 0, dst_mode = 0;
        if (info->num_operands == 2) {
            src_mode = detect_addressing_mode(operand1);
            dst_mode = detect_addressing_mode(operand2);
        } else if (info->num_operands == 1) {
            dst_mode = detect_addressing_mode(operand1);
        }

        int word = 0;
        word |= ((info->opcode & 0xF) << 6);
        if (info->num_operands == 2) {
            word |= ((src_mode & 0x3) << 4);
            word |= ((dst_mode & 0x3) << 2);
        } else if (info->num_operands == 1) {
            word |= ((dst_mode & 0x3) << 2);
        }
        word |= ARE_ABSOLUTE & 0x3;

        /* ✅ מניעת קידוד כפול */
        if (IC == last_encoded_ic && line_number == last_line_encoded) {
            printf("[SKIP] Duplicate instruction at line %d (already encoded at IC=%d)\n", line_number, IC);
            continue;
        }

        /* ✅ קידוד פקודה */
        add_memory_word(IC++, word, ARE_ABSOLUTE, 0, NULL);
        last_encoded_ic = IC - 1;
        last_line_encoded = line_number;

        printf("IC = %d | binary word = ", IC - 1);
        int i;
        for (i = 9; i >= 0; i--) printf("%d", (word >> i) & 1);
        printf(" | value = %d\n", word);

        int added = count_extra_words(src_mode, dst_mode, info->num_operands);
        if (info->num_operands == 2) {
            printf(">> Operand 1 adds %d word(s)\n", (src_mode == ADDR_MATRIX) ? 3 : 1);
            printf(">> Operand 2 adds %d word(s)\n", (dst_mode == ADDR_MATRIX) ? 3 : (dst_mode == ADDR_REGISTER && src_mode == ADDR_MATRIX) ? 0 : 1);
        } else if (info->num_operands == 1) {
            printf(">> Operand adds %d word(s)\n", added);
        }

        if (info->num_operands == 2) {
            if (src_mode == ADDR_REGISTER && dst_mode == ADDR_REGISTER) {
                int reg1 = operand1[1] - '0';
                int reg2 = operand2[1] - '0';
                int encoded = (reg1 << 6) | (reg2 << 2);
                add_memory_word(IC++, encoded, ARE_RELOCATABLE, 0, NULL);
                printf(">> Added both registers in 1 word\n");
            } else {
                encode_operand(operand1, src_mode, 1, line_number);
                encode_operand(operand2, dst_mode, 0, line_number);
            }
        } else if (info->num_operands == 1) {
            encode_operand(operand1, dst_mode, 0, line_number);
        }
    }

    ICF = IC;
    DCF = DC;
}


