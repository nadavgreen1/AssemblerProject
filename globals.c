/* globals.c */
/* Implementation of global variables for the assembler */

#include <stdlib.h>
#include "globals.h"

/* Instruction and data counters */
int IC = MEMORY_START;
int DC = 0;
int ICF = 0;
int DCF = 0;
/* Error flag to indicate whether any error occurred during processing */

int error_found = 0;


/* File names used during processing */
char *file_name = NULL;
char *am_file_name = NULL;
char *ob_file_name = NULL;
char *ent_file_name = NULL;
char *ext_file_name = NULL;
