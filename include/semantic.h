#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"
#include "parser.h"

#define TYPE_INT 0

// Basic symbol structure
typedef struct Symbol {
    char name[100];          // Variable name
    int type;                // Data type (int, etc.)
    int scope_level;         // Scope nesting level
    int line_declared;       // Line where declared
    int is_initialized;      // Has been assigned a value?
    struct Symbol* next;     // For linked list implementation
} Symbol;

// Symbol table
typedef struct {
    Symbol* head;            // First symbol in the table
    int current_scope;       // Current scope level
} SymbolTable;


// Initialize a new symbol table
// Creates an empty symbol table structure with scope level set to 0
SymbolTable* init_symbol_table();

// Add a symbol to the table
// Inserts a new variable with given name, type, and line number into the current scope
void add_symbol(SymbolTable* table, const char* name, int type, int line);

// Look up a symbol in the table
// Searches for a variable by name across all accessible scopes
// Returns the symbol if found, NULL otherwise
Symbol* lookup_symbol(SymbolTable* table, const char* name);

// Enter a new scope level
// Increments the current scope level when entering a block (e.g., if, while)
void enter_scope(SymbolTable* table);

// Exit the current scope
// Decrements the current scope level when leaving a block
// Optionally removes symbols that are no longer in scope
void exit_scope(SymbolTable* table);

// Remove symbols from the current scope
// Cleans up symbols that are no longer accessible after leaving a scope
void remove_symbols_in_current_scope(SymbolTable* table);

// Free the symbol table memory
// Releases all allocated memory when the symbol table is no longer needed
void free_symbol_table(SymbolTable* table);

// New semantic analysis definitions

typedef enum {
    SEM_ERROR_NONE,
    SEM_ERROR_UNDECLARED_VARIABLE,
    SEM_ERROR_REDECLARED_VARIABLE,
    SEM_ERROR_TYPE_MISMATCH,
    SEM_ERROR_UNINITIALIZED_VARIABLE,
    SEM_ERROR_INVALID_OPERATION,
    SEM_ERROR_INVALID_ARGUMENT,             // For invalid argument values
    SEM_ERROR_FUNCTION_CALL_NO_ARGUMENTS,     // For missing arguments in function call
    SEM_ERROR_FUNCTION_CALL_TOO_MANY_ARGUMENTS, // For too many arguments in function call
    SEM_ERROR_SEMANTIC_ERROR
} SemanticErrorType;

// Report semantic errors
void semantic_error(SemanticErrorType error, const char* name, int line);

// Special feature validation: validate function calls (e.g. factorial)
int check_function_call(ASTNode* node, SymbolTable* table);


// Semantic checking functions for tye checking and variable checking 
int check_declaration(ASTNode* node, SymbolTable* table);
int check_assignment(ASTNode* node, SymbolTable* table);
int check_expression(ASTNode* node, SymbolTable* table);