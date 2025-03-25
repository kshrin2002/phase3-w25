#include "../../include/semantic.h"

// Initializing new symbol table
SymbolTable* init_symbol_table() {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (table){
        table->head = NULL;
        table->current_scope = 0;
    }
    return table;
}

// Adding a symbol to the table
void add_symbol(SymbolTable* table, const char* name, int type, int line) {
    Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
    if (symbol) {
    strcpy(symbol->name, name);
    symbol->type = type;
    symbol->scope_level = table->current_scope;
    symbol->line_declared = line;
    symbol->is_initialized = 0;
    symbol->next = table->head;
    table->head = symbol;
    }   
}

// Looking up a symbol in the table
Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
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

// Semantic Error Reporting
void semantic_error(SemanticErrorType error, const char* name, int line) {
    printf("Semantic Error at line %d: ", line);
    switch (error) {
        case SEM_ERROR_UNDECLARED_VARIABLE:
            printf("Undeclared variable '%s'\n", name);
            break;
        case SEM_ERROR_REDECLARED_VARIABLE:
            printf("Variable '%s' already declared in this scope\n", name);
            break;
        case SEM_ERROR_TYPE_MISMATCH:
            printf("Type mismatch involving '%s'\n", name);
            break;
        case SEM_ERROR_UNINITIALIZED_VARIABLE:
            printf("Variable '%s' may be used uninitialized\n", name);
            break;
        case SEM_ERROR_INVALID_OPERATION:
            printf("Invalid operation involving '%s'\n", name);
            break;
        case SEM_ERROR_INVALID_ARGUMENT:
            printf("Invalid argument for function '%s'\n", name);
            break;
        case SEM_ERROR_FUNCTION_CALL_NO_ARGUMENTS:
            printf("Function '%s' call requires one argument, but none provided\n", name);
            break;
        case SEM_ERROR_FUNCTION_CALL_TOO_MANY_ARGUMENTS:
            printf("Function '%s' call has too many arguments\n", name);
            break;
        default:
            printf("Unknown semantic error with '%s'\n", name);
    }
}

// Expression Checking
int check_expression(ASTNode* node, SymbolTable* table) {
    return 1;
}

// Special Feature: Function Call Validation
int check_function_call(ASTNode* node, SymbolTable* table) {
    // Ensure node is function call
    if (node->type != AST_FUNCTIONCALL) {
        return 1;
    }

    // Validate function being called is "factorial"
    if (strcmp(node->token.lexeme, "factorial") != 0) {
        semantic_error(SEM_ERROR_INVALID_OPERATION, node->token.lexeme, node->token.line);
        return 0;
    }

    // Check exactly one argument provided
    if (node->args == NULL) {
        semantic_error(SEM_ERROR_FUNCTION_CALL_NO_ARGUMENTS, "factorial", node->token.line);
        return 0;
    }
    // if more than one argument is passed, "right" will not be NULL.
    if (node->args->right != NULL) {
        semantic_error(SEM_ERROR_FUNCTION_CALL_TOO_MANY_ARGUMENTS, "factorial", node->token.line);
        return 0;
    }

    // Validate argument expression
    int valid = check_expression(node->args, table);

    // If argument is a number literal, check that it's non-negative
    if (node->args->type == AST_NUMBER) {
        int value = atoi(node->args->token.lexeme);
        if (value < 0) {
            semantic_error(SEM_ERROR_INVALID_ARGUMENT, "factorial", node->token.line);
            valid = 0;
        }
    }

    return valid;
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

