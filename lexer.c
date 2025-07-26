/* lexer.c */
/* Token extraction and label validation */
#include "globals.h"
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "util.h"  /* For skip_whitespace */

const char *extract_operand(const char *cursor, char *buffer) {
    int i = 0;

    cursor = skip_whitespace(cursor);

    if (*cursor == '#') {
        buffer[i++] = *cursor++;
        while (isdigit(*cursor) || *cursor == '-' || *cursor == '+') {
            buffer[i++] = *cursor++;
        }
    } else if (isalpha(*cursor)) {
        while (isalnum(*cursor)) {
            buffer[i++] = *cursor++;
        }

        if (*cursor == '[') {
            int brackets = 0;
            do {
                buffer[i++] = *cursor;
                if (*cursor == '[') brackets++;
                if (*cursor == ']') brackets--;
                cursor++;
            } while (*cursor && brackets > 0 && i < MAX_LABEL_LENGTH - 1);
            if (*cursor == ']') buffer[i++] = *cursor++;
        }
    }

    buffer[i] = '\0';
    return cursor;
}


int is_label(const char *token) {
    size_t len = strlen(token);
    return len > 0 && token[len - 1] == ':';
}

int is_comment_or_empty(const char *line) {
    line = skip_whitespace(line);
    return (*line == '\0' || *line == ';');
}

int is_valid_label(const char *str) {
    return is_valid_label_name(str); /* השתמש בפונקציה מ-util.c */
}
void extract_token(char *dest, const char *src, int max_len) {
    int i = 0;

    src = skip_whitespace(src);
    while (*src && !isspace(*src) && *src != ',' && i < max_len - 1) {
        dest[i++] = *src++;
    }

    dest[i] = '\0';
}
