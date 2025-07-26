/* code_conversion.c */
/* Handles conversion of numbers to base-4 representation using a/b/c/d */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_conversion.h"

#define BASE4_DIGITS "abcd"
#define WORD_SIZE 10  /* Each encoded word is 10 bits */
#define BASE4_LENGTH 4

/* Converts a digit (0–3) to a base-4 character */
static char base4_char(int value) {
    if (value >= 0 && value < 4)
        return BASE4_DIGITS[value];
    return '?';
}

/* Converts a signed number to unsigned 10-bit representation */
int to_unsigned_10bit(int value) {
    return value & 0x3FF;
}




/**
 * @param num The number (can be negative or positive)
 * @param buffer Output string (must be at least 7 bytes)
 */


void to_base4(int value, char *output) {
    unsigned int uvalue = to_unsigned_10bit(value);
    int i;
    for (i = BASE4_LENGTH - 1; i >= 0; i--) {
        output[i] = 'a' + (uvalue % 4);
        uvalue /= 4;
    }
    output[BASE4_LENGTH] = '\0';
}





/**
 * Encodes address and code into base-4 (a–d) strings
 */
void encode_to_output_format(int address, int code, char *address_out, char *code_out) {
    to_base4(address, address_out);
    to_base4(code, code_out);
}
