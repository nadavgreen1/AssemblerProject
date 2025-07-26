/* util.c */
/* General utility functions for string and number handling */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "globals.h"

/**
 * Checks if a string is a valid label name.
 * Valid labels must start with a letter and contain only letters/digits.
 */
int is_valid_label_name(const char *label) {
    int i;
    if (!isalpha(label[0])) {
        return 0;
    }
    for (i = 1; label[i] != '\0'; i++) {
        if (!isalnum(label[i])) {
            return 0;
        }
    }
    return 1;
}

/**
 * Removes trailing newline or carriage return from a string, if present.
 */
void strip_newline(char *line) {
    size_t len = strlen(line);
    if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[len - 1] = '\0';
    }
    if (len > 1 && (line[len - 2] == '\r')) {
        line[len - 2] = '\0';
    }
}

/**
 * Returns a pointer to the first non-whitespace character in the input.
 */
/* util.c */
char *skip_whitespace(const char *line) {
    while (*line && isspace(*line)) {
        line++;
    }
    return (char *)line;
}



/**
 * Duplicates a string (like strdup but portable).
 */
char *string_dup(const char *str) {
    char *dup = (char *)malloc(strlen(str) + 1);
    if (dup) {
        strcpy(dup, str);
    }
    return dup;
}

char macro_names[MAX_MACROS][MAX_LABEL_LENGTH];
int macro_count = 0;

void add_macro_name(const char *name) {
    if (macro_count < MAX_MACROS) {
        strcpy(macro_names[macro_count++], name);
    }
}
int is_macro_call(const char *token) {
    int i;
    for (i = 0; i < macro_count; i++) {
        if (strcmp(macro_names[i], token) == 0) return 1;
    }
    return 0;
}

/**
 * Checks if a string represents a valid decimal integer (with optional + or -).
 */
int is_number(const char *str) {
    if (*str == '+' || *str == '-') {
        str++;
    }
    if (!*str) return 0; /* Empty after sign */

    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}
