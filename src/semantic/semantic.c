#include "../../include/semantic.h"

// Initializing new symbol table
SymbolTable* init_symbol_table() {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->head = NULL;
    table->current_scope = 0;
    return table;
}

// Adding a symbol to the table
void add_symbol(SymbolTable* table, const char* name, int type, int line) {
    Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
    strcpy(symbol->name, name);
    symbol->type = type;
    symbol->scope_level = table->current_scope;
    symbol->line_declared = line;
    symbol->is_initialized = 0;
    symbol->next = table->head;
    table->head = symbol;
}

// Looking up a symbol in the table
Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Entering a new scope level
void enter_scope(SymbolTable* table) {
    table->current_scope++;
}

// Exiting the current scope
void exit_scope(SymbolTable* table) {
    remove_symbols_in_current_scope(table);
    table->current_scope--;
}

// Look up a symbol in any accessible scope
Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL; // Symbol not found
}

// Removing symbols from the current scope
void remove_symbols_in_current_scope(SymbolTable* table) {
    Symbol* current = table->head;
    Symbol* prev = NULL;
    while (current != NULL) {
        if (current->scope_level > table->current_scope) {
            if (prev == NULL) {
                table->head = current->next;
            } else {
                prev->next = current->next;
            }
            Symbol* temp = current;
            current = current->next;
            free(temp);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

// Freeing the symbol table memory
void free_symbol_table(SymbolTable* table) {
    Symbol* current = table->head;
    while (current != NULL) {
        Symbol* temp = current;
        current = current->next;
        free(temp);
    }
    free(table);
}

// Main function for testing the symbol table
int main() {
    SymbolTable* table = init_symbol_table();
    add_symbol(table, "x", 0, 1);
    add_symbol(table, "y", 1, 2);
    add_symbol(table, "z", 2, 3);
    Symbol* sym = lookup_symbol(table, "y");
    if (sym != NULL) {
        printf("Symbol found: %s\n", sym->name);
    } else {
        printf("Symbol not found\n");
    }
    enter_scope(table);
    add_symbol(table, "a", 3, 4);
    add_symbol(table, "b", 4, 5);
    exit_scope(table);
    sym = lookup_symbol(table, "b");
    if (sym != NULL) {
        printf("Symbol found: %s\n", sym->name);
    } else {
        printf("Symbol not found\n");
    }
    free_symbol_table(table);
    return 0;
}

