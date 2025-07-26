#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>

/**
 * Performs the first pass over the source file.
 * This includes:
 * - Macro and label detection
 * - Storing symbols into the symbol table
 * - Calculating IC and DC
 *
 * @param source_file Pointer to the opened source file (.am)
 */
void first_pass(FILE *source_file);

#endif /* FIRST_PASS_H */
