/* memory_image.c */
#include <string.h>
#include <stdio.h>
#include "memory_image.h"
#include "globals.h"

MemoryWord memory_image[MAX_MEMORY_SIZE];
int memory_word_count = 0;

/**
 * Adds a new word to the memory image.
 */
void add_memory_word(int address, int value, int are, int is_external, const char *external_label) {
    int index = address - MEMORY_START;
    if (index >= 0 && index < MAX_MEMORY_SIZE) {
        if (index >= memory_word_count) {
            memory_word_count = index + 1; // שמור על count נכון גם אם יש קפיצה
        }

        memory_image[index].address = address;
        memory_image[index].value = value;
        memory_image[index].are = are;
        memory_image[index].is_external = is_external;

        memory_image[index].external_label[0] = '\0';
        if (is_external && external_label) {
            strncpy(memory_image[index].external_label, external_label, MAX_LABEL_LENGTH - 1);
            memory_image[index].external_label[MAX_LABEL_LENGTH - 1] = '\0';
        }
        printf("[DEBUG] ADD WORD: address=%d value=%d are=%d ext=%d label=%s\n",
       address, value, are, is_external,
       external_label ? external_label : "");


    } else {
        fprintf(stderr, "Error: memory image overflow at address %d\n", address);
    }
}

/**
 * Resets the memory image (useful between files).
 */
void reset_memory_image() {
    int i;
    for (i = 0; i < memory_word_count; i++) {
        memory_image[i].address = 0;
        memory_image[i].value = 0;
        memory_image[i].are = 0;
        memory_image[i].is_external = 0;
        memory_image[i].external_label[0] = '\0';
    }
    memory_word_count = 0;
}