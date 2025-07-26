/* preproc.c */
/* Handles macro expansion before the assembler passes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "lexer.h"
#include "util.h"
#include "error_handler.h"

#define MAX_MACRO_NAME 31
#define MAX_MACRO_BODY 1000

/* Structure to hold a macro definition */
typedef struct Macro {
    char name[MAX_MACRO_NAME];
    char body[MAX_MACRO_BODY];
    struct Macro *next;
} Macro;

static Macro *macro_list = NULL;

/* Adds a macro definition to the list */
void add_macro(const char *name, const char *body) {
    Macro *new_macro = (Macro *)malloc(sizeof(Macro));
    if (!new_macro) {
        printf("Memory allocation failed for macro\n");
        exit(1);
    }
    strncpy(new_macro->name, name, MAX_MACRO_NAME);
    new_macro->name[MAX_MACRO_NAME - 1] = '\0';
    strncpy(new_macro->body, body, MAX_MACRO_BODY);
    new_macro->body[MAX_MACRO_BODY - 1] = '\0';
    new_macro->next = macro_list;
    macro_list = new_macro;
}

/* Finds a macro by name */
Macro *find_macro(const char *name) {
    Macro *current = macro_list;
    while (current) {
        if (strcmp(current->name, name) == 0) return current;
        current = current->next;
    }
    return NULL;
}

/* Frees the entire macro list */
void free_macros() {
    Macro *current = macro_list;
    while (current) {
        Macro *next = current->next;
        free(current);
        current = next;
    }
    macro_list = NULL;
}

/* Handles macro expansion: reads .as and writes .am */
void preprocess_macros(FILE *input, FILE *output) {
    char line[MAX_LINE_LENGTH];
    char macro_body[MAX_MACRO_BODY];
    char macro_name[MAX_MACRO_NAME];
    int inside_macro = 0;

    macro_body[0] = '\0';

    while (fgets(line, MAX_LINE_LENGTH, input)) {
        const char *cursor = line;
        char token[MAX_MACRO_NAME];

        skip_whitespace(&cursor);

        if (is_comment_or_empty(cursor)) {
            if (!inside_macro) fputs(line, output);
            continue;
        }

        extract_token(token, cursor, MAX_MACRO_NAME);

        if (strcmp(token, "mcro") == 0) {
            inside_macro = 1;
            cursor += strlen(token);
            cursor = skip_whitespace(cursor);
            extract_token(macro_name, cursor, MAX_MACRO_NAME);
            macro_body[0] = '\0';
            continue;
        }

        if (strcmp(token, "mcroend") == 0) {
            if (inside_macro) {
                add_macro(macro_name, macro_body);
                inside_macro = 0;
            }
            continue;
        }

        if (inside_macro) {
            strncat(macro_body, line, MAX_MACRO_BODY - strlen(macro_body) - 1);
            continue;
        }

        Macro *macro = find_macro(token);
        if (macro) {
            fputs(macro->body, output);
        } else {
            fputs(line, output);
        }
    }

    free_macros();
}


/* Public function to handle preprocessing from .as to .am */
FILE *run_preprocessor(const char *filename) {
    char input_file[FILENAME_MAX];
    char output_file[FILENAME_MAX];
    FILE *in = NULL, *out = NULL;
    size_t len = strlen(filename);

    snprintf(input_file, FILENAME_MAX, "%s.as", filename);
    snprintf(output_file, FILENAME_MAX, "%s.am", filename);

    in = fopen(input_file, "r");
    if (!in) {
        report_general_error("Failed to open input file");
        return NULL;
    }

    out = fopen(output_file, "w");
    if (!out) {
        char msg[200];
        snprintf(msg, sizeof(msg), "Failed to open input file: %s", filename);
        report_general_error(msg);
        fclose(in);
        return NULL;
    }

    preprocess_macros(in, out);

    fclose(in);
    fclose(out);

    return fopen(output_file, "r");
}
