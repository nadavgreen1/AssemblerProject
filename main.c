#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "first_pass.h"
#include "second_pass.h"
#include "file_writer.h"
#include "error_handler.h"
#include "preproc.h"
#include "symbol_table.h"
#include "memory_image.h"

#define MAX_FILENAME_LEN 100

int main(int argc, char *argv[]) {
    int i;
    FILE *am_file;
    char am_file_name[FILENAME_MAX];

    if (argc < 2) {
        printf("Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        error_found = 0;
        file_name = argv[i];

        if (!run_preprocessor(file_name)) {
            fprintf(stderr, "Error: Preprocessing failed for file: %s.as\n", file_name);
            continue;
        }

        snprintf(am_file_name, FILENAME_MAX, "%s.am", file_name);

        am_file = fopen(am_file_name, "r");
        if (!am_file) {
            fprintf(stderr, "Error: Cannot open .am file: %s\n", am_file_name);
            continue;
        }

        printf("Preprocessing complete. Starting first pass for %s...\n", file_name);
        first_pass(am_file);
        fclose(am_file);

        am_file = fopen(am_file_name, "r");
        if (!am_file) {
            fprintf(stderr, "Error: Cannot reopen .am file for second pass: %s\n", am_file_name);
            continue;
        }

        if (!error_found) {
            printf("Starting second pass for %s...\n", file_name);
            second_pass(am_file);
        }
        fclose(am_file);

        if (!error_found) {
            write_output_files(file_name);
        }

        reset_memory_image();
        free_symbol_table();
    }

    return 0;
}
