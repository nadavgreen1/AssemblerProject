#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_LABEL_LENGTH 31

/* Enumeration for symbol types */
typedef enum {
    CODE_SYMBOL,
    DATA_SYMBOL,
    EXTERNAL_SYMBOL,
    ENTRY_SYMBOL
} SymbolType;

/* Symbol table entry structure */
typedef struct Symbol {
    char name[MAX_LABEL_LENGTH];
    int address;
    SymbolType type;
    int is_entry;
    struct Symbol *next;
} Symbol;

/* Returns the head of the symbol table */
Symbol *get_symbol_table_head();

/* Symbol table operations */
void add_symbol(const char *name, int address, SymbolType type);
Symbol *find_symbol(const char *name);
void mark_entry_symbol(const char *name);
void update_data_symbols(int offset);
void free_symbol_table();

#endif
