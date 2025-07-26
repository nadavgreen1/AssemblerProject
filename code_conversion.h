/* code_conversion.h */
/* Interface for base-4 (A/B/C/D) code conversion functions */

#ifndef CODE_CONVERSION_H
#define CODE_CONVERSION_H
int to_unsigned_10bit(int value);
/**
 * Converts a number to a base-4 string (A/B/C/D) representation.
 * @param num The number to convert
 * @param buffer Output buffer (must be at least 6 characters)
 */
void to_base4(int num, char *buffer);

/**
 * Encodes an address and binary code into base-4 strings.
 * @param address Memory address
 * @param code 10-bit binary instruction
 * @param address_out Output buffer for base-4 address (at least 6 chars)
 * @param code_out Output buffer for base-4 code (at least 6 chars)
 */
void encode_to_output_format(int address, int code, char *address_out, char *code_out);

#endif
