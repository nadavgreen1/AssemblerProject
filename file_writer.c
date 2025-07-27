/* file_writer.c */
/* Handles writing of output files after both passes.
 * The .ob file stores each memory word as a 12-bit value where the
 * two least significant bits are the ARE flags. */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symbol_table.h"
#include "file_writer.h"
#include "memory_image.h"

void to_base4_string(int value, char *output) {
    int i;
    unsigned int mask = 0x3FF; /* 10 bits mask */
    value &= mask; /* force value to be treated as unsigned 10-bit */
    for (i = 4; i >= 0; i--) {
        output[i] = 'a' + (value % 4);
        value /= 4;
    }
    output[5] = '\0';
}
void to_base4_trimmed(int value, char *output) {
    char full[6];
    int i = 0, j = 0;
    to_base4_string(value, full);

    /* דילוג על תווים 'a' מובילים */
    while (full[i] == 'a' && i < 4) i++;

    /* העתקה מהתווים הרלוונטיים */
    while (i < 5) output[j++] = full[i++];
    output[j] = '\0';
}


void write_output_files(const char *base_file_name) {
    FILE *ent_fp = NULL, *ob_fp = NULL, *ext_fp = NULL;
    Symbol *current = NULL;
    char full_name[MAX_FILE_NAME];
    int i, ext_used = 0;

    char addr_base4[6], val_base4[6]; /* 5 תווים + null terminator */

    /* === .ent file === */
    snprintf(full_name, MAX_FILE_NAME, "%s.ent", base_file_name);
    ent_fp = fopen(full_name, "w");
    if (ent_fp) {
        current = get_symbol_table_head();
        while (current) {
            if (current->type == ENTRY_SYMBOL) {
                to_base4_string(current->address, addr_base4);
                fprintf(ent_fp, "%s %s\n", current->name, addr_base4);
            }
            current = current->next;
        }
        fclose(ent_fp);
    }

    /* === .ob file === */
    snprintf(full_name, MAX_FILE_NAME, "%s.ob", base_file_name);
    ob_fp = fopen(full_name, "w");
    if (!ob_fp) {
        printf("Failed to create .ob file\n");
        return;
    }

    /* Header: code size and data size – as regular numbers */
    char code_size_base4[6], data_size_base4[6];
    to_base4_trimmed(ICF - MEMORY_START, code_size_base4);
    to_base4_trimmed(DCF, data_size_base4);
    fprintf(ob_fp, "%s %s\n", code_size_base4, data_size_base4);





    /* Write CODE words */
    for (i = 0; i < IC - MEMORY_START; i++) {
        /*
         * According to the expected .ob format the address column should not
         * contain a leading 'a'. We therefore print addresses using the
         * trimmed variant while keeping the values padded to five characters.
         */
        to_base4_trimmed(memory_image[i].address, addr_base4);
        int encoded = memory_image[i].value;
        if ((encoded & 0x3) != memory_image[i].are) {
            encoded = ((encoded & 0x3FF) << 2) | (memory_image[i].are & 0x3);
        }
        to_base4_string(encoded, val_base4);
        fprintf(ob_fp, "%s %s\n", addr_base4, val_base4);
        printf("MEM[%d] = %d\n", memory_image[i].address, encoded);

    }

    /* Write DATA words */
    for (i = IC - MEMORY_START; i < memory_word_count; i++) {
        to_base4_trimmed(memory_image[i].address, addr_base4);
        /* Data words are stored as raw 10-bit values without ARE bits */
        int encoded = memory_image[i].value & 0x3FF;
        to_base4_string(encoded, val_base4);
        fprintf(ob_fp, "%s %s\n", addr_base4, val_base4);
    }

    fclose(ob_fp);

    /* === .ext file === */
    snprintf(full_name, MAX_FILE_NAME, "%s.ext", base_file_name);
    ext_fp = fopen(full_name, "w");
    if (ext_fp) {
        for (i = 0; i < memory_word_count; i++) {
            if (memory_image[i].is_external) {
                to_base4_string(memory_image[i].address, addr_base4);
                fprintf(ext_fp, "%s %s\n", memory_image[i].external_label, addr_base4);
                ext_used = 1;
            }
        }
        fclose(ext_fp);
        if (!ext_used) remove(full_name); /* Delete if not used */
    }
    for (i = 0; i < memory_word_count; i++) {
        if (i > memory_word_count - 5) {
            printf("DEBUG: addr %d → %d\n", memory_image[i].address, memory_image[i].value);
        }
    }

    printf("All output files written successfully.\n");
}
