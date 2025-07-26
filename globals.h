/* globals.h - Global variables and constants shared across the assembler */

#ifndef GLOBALS_H
#define GLOBALS_H

/* Constants */
#define MAX_LINE_LENGTH 81     /* 80 chars + '\0' */
#define MAX_LABEL_LENGTH 31    /* Label name max length */
#define MEMORY_START 100       /* Code & Data section start address */
#define MAX_FILE_NAME 100

/* IC and DC counters */
extern int IC;
extern int DC;
extern int ICF;
extern int DCF;
/* Error flag */
extern int error_found;


/* File names */
extern char *file_name;       /* Input file name (without extension) */
extern char *am_file_name;    /* Macro-processed file (.am) */
extern char *ob_file_name;    /* Object file name (.ob) */
extern char *ent_file_name;   /* Entry file name (.ent) */
extern char *ext_file_name;   /* External file name (.ext) */

#endif
