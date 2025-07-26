/* error_handler.c */
/* Handles error reporting for the assembler */

#include <stdio.h>
#include "error_handler.h"
#include "globals.h"

/**
 * Reports an error at a specific line and sets the global error flag.
 * @param message The error message to display
 * @param line_number The line where the error occurred
 */
void report_error(const char *message, int line_number) {
    printf("Error on line %d: %s\n", line_number, message);
    error_found = 1;
}

/**
 * Reports a general error (not tied to a specific line).
 * @param message The error message to display
 */
void report_general_error(const char *message) {
    printf("Error: %s\n", message);
    error_found = 1;
}
