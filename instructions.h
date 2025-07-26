#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

typedef enum {
    ADDR_IMMEDIATE = 0,
    ADDR_DIRECT = 1,
    ADDR_REGISTER = 2,
    ADDR_MATRIX = 3
} AddressingMode;

typedef struct {
    const char *name;
    int opcode;
    int num_operands;
} InstructionInfo;

InstructionInfo *get_instruction_info(const char *mnemonic);
int detect_addressing_mode(const char *operand);
void encode_operand(const char *operand, int mode, int is_source, int line_number);
int is_register(const char *token);

#endif
