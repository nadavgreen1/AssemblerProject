#ifndef LEXER_H
#define LEXER_H

#define MAX_LINE_LENGTH 81
#define MAX_TOKEN_LENGTH 32

void extract_token(char *dest, const char *src, int max_len);
int is_label(const char *token);
int is_comment_or_empty(const char *line);
int is_valid_label(const char *str);
const char *extract_operand(const char *cursor, char *buffer);

#endif
