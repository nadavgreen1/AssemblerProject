/* util.h */
/* Utility function declarations */

#ifndef UTIL_H
#define UTIL_H
#define MAX_MACROS 100
void add_macro_name(const char *name);
int is_macro_call(const char *token);
int is_valid_label_name(const char *label);
void strip_newline(char *line);
int is_number(const char *str);

/**
 * Returns a pointer to the first non-whitespace character in the input string.
 */
char *skip_whitespace(const char *line);

char *string_dup(const char *str);

#endif
