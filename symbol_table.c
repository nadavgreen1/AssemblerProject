#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "error_handler.h"

static Symbol *symbol_table_head = NULL;

void add_symbol(const char *name, int address, SymbolType type) {
    Symbol *existing = find_symbol(name);
    if (existing) {
        if (existing->type == EXTERNAL_SYMBOL && type == ENTRY_SYMBOL) {
            existing->type = ENTRY_SYMBOL;
            existing->is_entry = 1;
            return;
        }
        report_general_error("Duplicate label definition");
        return;
    }

    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    if (!new_symbol) {
        printf("Memory allocation failed for symbol: %s\n", name);
        exit(1);
    }

    strncpy(new_symbol->name, name, MAX_LABEL_LENGTH - 1);
    new_symbol->name[MAX_LABEL_LENGTH - 1] = '\0';
    new_symbol->address = address;
    new_symbol->type = type;
    new_symbol->is_entry = 0; /* ← אתחול ברירת מחדל */
    new_symbol->next = symbol_table_head;
    symbol_table_head = new_symbol;
}

Symbol *find_symbol(const char *name) {
    Symbol *current = symbol_table_head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void mark_entry_symbol(const char *name) {
    Symbol *sym = find_symbol(name);
    if (sym) {
        sym->type = ENTRY_SYMBOL;
        sym->is_entry = 1;
    } else {
        report_error("Undefined symbol in .entry", 0);  /* שגיאה בלי שורת קלט */
    }
}

void update_data_symbols(int offset) {
    Symbol *current = symbol_table_head;
    while (current) {
        if (current->type == DATA_SYMBOL) {
            current->address += offset;
        }
        current = current->next;
    }
}

Symbol *get_symbol_table_head() {
    return symbol_table_head;
}

void free_symbol_table() {
    Symbol *current = symbol_table_head;
    while (current) {
        Symbol *next = current->next;
        free(current);
        current = next;
    }
    symbol_table_head = NULL;
}
