/* first_pass.c */
/* Handles the first pass of the assembler */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "first_pass.h"
#include "globals.h"
#include "lexer.h"
#include "symbol_table.h"
#include "error_handler.h"
#include "util.h"
#include "memory_image.h"
#include "instructions.h"

#define MAX_OPERANDS 100

void store_data_word(int value) {
    printf("[DATA DEBUG] store_data_word: address = %d, value = %d\n", IC + DC, value);
    add_memory_word(IC + DC, value, ARE_ABSOLUTE, 0, NULL);
    DC++;
}


void store_string(const char *str) {
    while (*str) {
        store_data_word((int)(*str));
        str++;
    }
    store_data_word(0); /* null terminator */
}

void parse_data_directive(const char *cursor, int line_number) {
    char token[MAX_TOKEN_LENGTH];
    while (*cursor) {
        cursor = skip_whitespace(cursor);
        extract_token(token, cursor, MAX_TOKEN_LENGTH);
        if (strlen(token) == 0) break;
        if (!is_number(token)) {
            report_error("Invalid number in .data", line_number);
            return;
        }
        store_data_word(atoi(token));
        cursor += strlen(token);
        cursor = skip_whitespace(cursor);
        if (*cursor == ',') cursor++;
    }
}

int parse_mat_directive(const char *cursor, int line_number) {
    int rows = 0, cols = 0, count = 0;
    char token[MAX_TOKEN_LENGTH];

    if (sscanf(cursor, " [%d][%d]", &rows, &cols) != 2) {
        report_error("Invalid matrix format. Expected [rows][cols]", line_number);
        return 0;
    }

    int total = rows * cols;
    if (total <= 0 || total > MAX_OPERANDS) {
        report_error("Invalid matrix size", line_number);
        return 0;
    }

    const char *pos = strchr(cursor, ']');
    if (!pos || !(pos = strchr(pos + 1, ']'))) {
        report_error("Malformed matrix brackets", line_number);
        return 0;
    }
    cursor = pos + 1;

    while (count < total) {
        cursor = skip_whitespace(cursor);
        extract_token(token, cursor, MAX_TOKEN_LENGTH);
        if (strlen(token) == 0) break;
        printf("[MATRIX DEBUG] Added value %d at DC %d\n", atoi(token), DC);

        if (!is_number(token)) {
            report_error("Invalid number in .data", line_number);
            return 0;
        }

        store_data_word(atoi(token));
        count++;
        cursor += strlen(token);
        cursor = skip_whitespace(cursor);
        if (*cursor == ',') cursor++;
    }


    if (count != total) {
        report_error("Matrix values count doesn't match declared size", line_number);
    }
    return count;
}

int calc_instruction_size(InstructionInfo *info, const char *op1, const char *op2) {
    int size = 1;
    if (info->num_operands == 0) return size;

    int mode1 = 0, mode2 = 0;
    if (info->num_operands >= 1) mode1 = detect_addressing_mode(op1);
    if (info->num_operands == 2) mode2 = detect_addressing_mode(op2);

    if (info->num_operands == 2 && mode1 == ADDR_REGISTER && mode2 == ADDR_REGISTER) {
        size += 1;
    } else {
        if (info->num_operands >= 1)
            size += (mode1 == ADDR_MATRIX) ? 3 : 1;
        if (info->num_operands == 2)
            size += (mode2 == ADDR_MATRIX) ? 3 : 1;
    }

    return size;
}

void first_pass(FILE *source_file) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    IC = MEMORY_START;
    DC = 0;

    while (fgets(line, MAX_LINE_LENGTH, source_file)) {
        const char *cursor = line;
        char label[MAX_LABEL_LENGTH] = "";
        char token[MAX_TOKEN_LENGTH];
        char op1[MAX_TOKEN_LENGTH], op2[MAX_TOKEN_LENGTH];

        line_number++;
        cursor = skip_whitespace(cursor);
        if (is_comment_or_empty(cursor)) continue;

        const char *colon_ptr = strchr(cursor, ':');
        if (colon_ptr && colon_ptr - cursor < MAX_LABEL_LENGTH) {
            strncpy(token, cursor, colon_ptr - cursor);
            token[colon_ptr - cursor] = '\0';

            if (!is_valid_label(token)) {
                report_error("Invalid label", line_number);
                continue;
            }

            strcpy(label, token);
            cursor = colon_ptr + 1;
            cursor = skip_whitespace(cursor);
            extract_token(token, cursor, MAX_TOKEN_LENGTH);  /* Extract directive or instruction */
        } else {
            extract_token(token, cursor, MAX_TOKEN_LENGTH);  /* No label found, extract token as usual */
        }

        /* Handle directives */
        if (strcmp(token, ".data") == 0) {
            if (label[0]) {
                if (find_symbol(label)) {
                    report_error("Duplicate label", line_number);
                    continue;
                }
                add_symbol(label, IC + DC, DATA_SYMBOL);
            }
            if (is_macro_call(token)) {
                /* Skip macro calls in first pass – their lines already expanded */
                continue;
            }
            cursor += strlen(token);
            parse_data_directive(cursor, line_number);
        }
        else if (strcmp(token, ".string") == 0) {
            if (label[0]) {
                if (find_symbol(label)) {
                    report_error("Duplicate label", line_number);
                    continue;
                }
                add_symbol(label, IC + DC, DATA_SYMBOL);
            }
            if (is_macro_call(token)) {
                /* Skip macro calls in first pass – their lines already expanded */
                continue;
            }
            cursor += strlen(token);
            cursor = skip_whitespace(cursor);
            if (*cursor == '"') {
                cursor++;
                const char *end = strchr(cursor, '"');
                if (!end) {
                    report_error("Unterminated string", line_number);
                    continue;
                }
                char str[MAX_LINE_LENGTH];
                strncpy(str, cursor, end - cursor);
                str[end - cursor] = '\0';
                store_string(str);
            } else {
                report_error("Missing opening quote for .string", line_number);
            }
        }
        else if (strcmp(token, ".mat") == 0) {
            if (label[0]) {
                if (find_symbol(label)) {
                    report_error("Duplicate label", line_number);
                    continue;
                }
                add_symbol(label, IC + DC, DATA_SYMBOL);
            }
            if (is_macro_call(token)) {
                /* Skip macro calls in first pass – their lines already expanded */
                continue;
            }

            cursor += strlen(token);
            cursor = skip_whitespace(cursor);

            parse_mat_directive(cursor, line_number);
        }
        else if (strcmp(token, ".extern") == 0) {
            cursor += strlen(token);
            cursor = skip_whitespace(cursor);
            extract_token(token, cursor, MAX_TOKEN_LENGTH);
            if (!is_valid_label(token)) {
                report_error("Invalid label in .extern", line_number);
                continue;
            }
            add_symbol(token, 0, EXTERNAL_SYMBOL);
        }
        else if (strcmp(token, ".entry") == 0) {
            /* Entry handled in second pass */
            continue;
        }
        else {
            InstructionInfo *info = get_instruction_info(token);
            if (!info) {
                report_error("Unknown command", line_number);
                continue;
            }

            if (label[0]) {
                if (find_symbol(label)) {
                    report_error("Duplicate label", line_number);
                    continue;
                }
                add_symbol(label, IC, CODE_SYMBOL);
            }

            cursor += strlen(token);
            cursor = skip_whitespace(cursor);

            op1[0] = op2[0] = '\0';

            if (info->num_operands >= 1) {
                extract_token(op1, cursor, MAX_TOKEN_LENGTH);
                if (strlen(op1) == 0) {
                    report_error("Missing first operand", line_number);
                    continue;
                }
                cursor += strlen(op1);
                cursor = skip_whitespace(cursor);
            }

            if (info->num_operands == 2) {
                if (*cursor != ',') {
                    report_error("Missing comma between operands", line_number);
                    continue;
                }
                cursor++;
                cursor = skip_whitespace(cursor);
                extract_token(op2, cursor, MAX_TOKEN_LENGTH);
                if (strlen(op2) == 0) {
                    report_error("Missing second operand", line_number);
                    continue;
                }
            }

            IC += calc_instruction_size(info, op1, op2);
        }
    }

#ifdef DEBUG
    Symbol *sym = get_symbol_table_head();
    while (sym) {
        printf(">> %s (addr %d, type %d)\n", sym->name, sym->address, sym->type);
        sym = sym->next;
    }
#endif

    update_data_symbols(IC);
}
