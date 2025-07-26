/* instructions.c */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "instructions.h"
#include "memory_image.h"
#include "symbol_table.h"
#include "error_handler.h"
#include "globals.h"  /* נדרש עבור IC */
#include "code_conversion.h"
#include "file_writer.h"

static InstructionInfo instruction_table[] = {
    {"mov", 0, 2},
    {"cmp", 1, 2},
    {"add", 2, 2},
    {"sub", 3, 2},
    {"not", 4, 1},
    {"clr", 5, 1},
    {"lea", 6, 2},
    {"inc", 7, 1},
    {"dec", 8, 1},
    {"jmp", 9, 1},
    {"bne", 10, 1},
    {"red", 11, 1},
    {"prn", 12, 1},
    {"jsr", 13, 1},
    {"rts", 14, 0},
    {"stop", 15, 0},
    {NULL, -1, 0}
};

InstructionInfo *get_instruction_info(const char *mnemonic) {
    int i = 0;
    while (instruction_table[i].name) {
        if (strcmp(instruction_table[i].name, mnemonic) == 0) {
            return &instruction_table[i];
        }
        i++;
    }
    return NULL;
}

int is_register(const char *token) {
    return strlen(token) == 2 && token[0] == 'r' && token[1] >= '0' && token[1] <= '7';
}

int detect_addressing_mode(const char *operand) {
    if (!operand || strlen(operand) == 0)
        return -1;

    if (operand[0] == '#') return ADDR_IMMEDIATE;
    if (is_register(operand)) return ADDR_REGISTER;

    /* Detect matrix: label[rX][rY] exactly */
    const char *bracket1 = strchr(operand, '[');
    const char *middle = strstr(operand, "][");
    const char *bracket2 = strrchr(operand, ']');

    if (bracket1 && middle && bracket2 && bracket1 < middle && middle < bracket2) {
        return ADDR_MATRIX;
    }

    return ADDR_DIRECT;
}

void encode_operand(const char *operand, int mode, int is_source, int line_number) {
    if (mode == ADDR_IMMEDIATE) {
        int value = atoi(operand + 1);
        printf("Before IC: %d\n", IC);

        add_memory_word(IC++, to_unsigned_10bit(value), ARE_ABSOLUTE, 0, NULL);
        printf("After IC: %d\n", IC);

    } else if (mode == ADDR_DIRECT) {
        Symbol *sym = find_symbol(operand);
        if (!sym) {
            report_error("Undefined label in operand", line_number);
            return;
        }
        int is_ext = (sym->type == EXTERNAL_SYMBOL);
        int are_bits = is_ext ? ARE_EXTERNAL : ARE_RELOCATABLE;
        printf("Before IC: %d\n", IC);

        add_memory_word(IC++, sym->address & 0x3FF, are_bits, is_ext, is_ext ? sym->name : NULL);
        printf("After IC: %d\n", IC);

    } else if (mode == ADDR_REGISTER) {
        int reg_num = operand[1] - '0';
        int encoded = is_source ? (reg_num << 6) : (reg_num << 2);
        printf("Before IC: %d\n", IC);

        add_memory_word(IC++, encoded & 0x3FF, ARE_RELOCATABLE, 0, NULL);
        printf("After IC: %d\n", IC);

    } else if (mode == ADDR_MATRIX) {
        char label[MAX_LABEL_LENGTH], reg1[4], reg2[4];
        const char *p = operand;
        int i = 0;

        while (*p && *p != '[' && i < MAX_LABEL_LENGTH - 1) {
            label[i++] = *p++;
        }
        label[i] = '\0';

        if (*p != '[') {
            report_error("Invalid matrix syntax: missing [", line_number);
            return;
        }
        p++;  /* skip '[' */
        if (*p != 'r' || !isdigit(p[1]) || p[2] != ']') {
            report_error("Invalid row register in matrix", line_number);
            return;
        }
        reg1[0] = p[0]; reg1[1] = p[1]; reg1[2] = '\0';
        p += 3;

        if (*p != '[') {
            report_error("Invalid matrix syntax: missing second [", line_number);
            return;
        }
        p++;
        if (*p != 'r' || !isdigit(p[1]) || p[2] != ']') {
            report_error("Invalid column register in matrix", line_number);
            return;
        }
        reg2[0] = p[0]; reg2[1] = p[1]; reg2[2] = '\0';

        Symbol *sym = find_symbol(label);
        if (!sym) {
            report_error("Undefined matrix label", line_number);
            return;
        }
        int is_ext = sym->type == EXTERNAL_SYMBOL;
        int are_bits = is_ext ? ARE_EXTERNAL : ARE_RELOCATABLE;
        printf("Before IC: %d\n", IC);

        add_memory_word(IC++, sym->address & 0x3FF, are_bits, is_ext, is_ext ? sym->name : NULL);
        printf("After IC: %d\n", IC);

        int r1 = reg1[1] - '0';
        int r2 = reg2[1] - '0';
        int encoded = (r1 << 6) | (r2 << 2);

        printf("Before IC: %d\n", IC);

        add_memory_word(IC++, encoded & 0x3FF, ARE_RELOCATABLE, 0, NULL);

        printf("After IC: %d\n", IC);

    }
}
