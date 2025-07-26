/* memory_image.h */
#ifndef MEMORY_IMAGE_H
#define MEMORY_IMAGE_H

#define MAX_MEMORY_SIZE 1024
#define MAX_LABEL_LENGTH 31

/* ARE values */
#define ARE_ABSOLUTE 0
#define ARE_EXTERNAL 1
#define ARE_RELOCATABLE 2

typedef struct {
    int address;                        /* Memory address */
    int value;                          /* Value without ARE bits */
    int are;                            /* ARE bits: 0 - A, 1 - E, 2 - R */
    int is_external;                    /* 1 if word references external label */
    char external_label[MAX_LABEL_LENGTH]; /* Name of external label if used */
} MemoryWord;

extern MemoryWord memory_image[MAX_MEMORY_SIZE];
extern int memory_word_count;

void add_memory_word(int address, int value, int are, int is_external, const char *external_label);
void reset_memory_image();
void update_data_word_addresses(int icf);

#endif /* MEMORY_IMAGE_H */
