#include "../../include/semantic.h"

// Declare functions to resolve circular dependencies
int check_statement(ASTNode* node, SymbolTable* table);
int check_block(ASTNode* node, SymbolTable* table);
int check_condition(ASTNode* node, SymbolTable* table);
int check_expression(ASTNode* node, SymbolTable* table);


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

// Look up symbol by name
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

// Look up symbol in current scope only
Symbol* lookup_symbol_current_scope(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0 && 
            current->scope_level == table->current_scope) {
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

// Expression Checking - commented out as this was a placeholder, it has now been implemented below.
// int check_expression(ASTNode* node, SymbolTable* table) {
//    return 1;
// }

// check a condition (e.g., in if/while statements)
int check_condition(ASTNode* node, SymbolTable* table) {
    if (node == NULL) {
        return 0;  // invalid if condition is NULL
    }

    // validate the condition expression
    int condition_type = check_expression(node, table);
    
    // condition must be a valid expression that resolves to an integer
    if (condition_type == -1) {
        return 0;
    }

    // conditions should resolve to an integer type
    if (condition_type != TYPE_INT) {
        semantic_error(SEM_ERROR_TYPE_MISMATCH, "condition", node->token.line);
        return 0;
    }

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


// check the expression for type correctness
int check_expression(ASTNode* node, SymbolTable* table) {
    if (node == NULL) {
        return -1;
    }

    switch (node->type) {
        case AST_NUMBER: // number literal is type int
            return TYPE_INT;

        case AST_IDENTIFIER:{
            Symbol* symbol = lookup_symbol(table, node->token.lexeme);
            if (!symbol) {
                semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, node->token.lexeme, node->token.line);
                return -1;
            }
            // check if the variable has been initialized, warn.
            if (!symbol->is_initialized) {
                semantic_error(SEM_ERROR_UNINITIALIZED_VARIABLE, node->token.lexeme, node->token.line);
                return symbol->type;
            }
            return symbol->type;
        }
        case AST_BINOP: {
            int left_type = check_expression(node->left, table);
            int right_type = check_expression(node->right, table);
            if (left_type == -1 || right_type == -1) {
                return -1;
            }
            // check if the types of the left and right expressions are the same
            if (left_type != right_type) {
                semantic_error(SEM_ERROR_TYPE_MISMATCH, node->token.lexeme, node->token.line);
                return -1;
            }
            return left_type;
        }
        case AST_FUNCTIONCALL: {
            // validate function calls, like factorial and such...
            int valid = check_function_call(node, table);
            if (!valid) {
                return -1;
            }
            // assume valid function call returns an integer
            return TYPE_INT;
        }

        default: // error for unsupported expression types
            return -1;
    }
}


// check variable declaration
int check_declaration(ASTNode* node, SymbolTable* table) {
    if (node == NULL || node->type != AST_VARDECL) { // check if node is a variable declaration
        return 1;
    }

    const char* variable_name = node->token.lexeme; // get the variable name
    
    // check if the variable has already been declared
    Symbol* current = table->head;
    while (current != NULL) {
        if (strcmp(current->name, variable_name) == 0 && current->scope_level == table->current_scope) { // Here is the check for the existence of the variable
            semantic_error(SEM_ERROR_REDECLARED_VARIABLE, variable_name, node->token.line);
            return -1; // return -1 if the variable has already been declared
        }
        current = current->next;
    }

    // add the variable to the symbol table
    add_symbol(table, variable_name, TYPE_INT, node->token.line);
    return TYPE_INT; // return the data type of the variable
}

// check variable assignment
int check_assignment(ASTNode* node, SymbolTable* table) {
    if (node == NULL || node->type != AST_ASSIGN || node->left == NULL || node->right == NULL) { 
        return -1;
    }

    const char* variable_name = node->left->token.lexeme; // get the variable name

    // look up the variable in the symbol table
    Symbol* symbol = lookup_symbol(table, variable_name);
    if (!symbol) { 
        semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, variable_name, node->left->token.line);
        return -1;
    }

    // check if the type of the right hand side matches the type of the variable
    int expression_type = check_expression(node->right, table);
    if (expression_type == -1) {
        return -1;
    }
    
    // check the variable's type and the expression type are compatible
    if (symbol->type != expression_type) {
        semantic_error(SEM_ERROR_TYPE_MISMATCH, variable_name, node->left->token.line);
        return -1;
    }

    // mark the variable as initialized
    symbol->is_initialized = 1;
    return expression_type;
}

// check a block of statements, handling scope
int check_block(ASTNode* node, SymbolTable* table) {
    if (node == NULL || node->type != AST_BLOCK) {
        return 1;  // if not a block, return success
    }

    // enter a new scope for the block
    enter_scope(table);

    // validate the statements within the block
    int valid = check_statement(node->left, table);

    // exit the scope, removing block-level variables
    exit_scope(table);

    return valid;
}


// enhanced check_statement to improve flow control validation
int check_statement(ASTNode* node, SymbolTable* table) {
    if (node == NULL) {
        return 1;
    }

    int valid = 1;

    switch (node->type) {
        case AST_VARDECL:
            valid &= (check_declaration(node, table) != -1);
            break;

        case AST_ASSIGN:
            valid &= (check_assignment(node, table) != -1);
            break;
    
        case AST_PRINT:
            valid &= (check_expression(node->left, table) != -1);
            break;
    
        case AST_BLOCK:
            valid &= check_block(node, table);
            break;
    
        case AST_IF: {
            // validate condition
            valid &= check_condition(node->left, table);
            
            // validate then branch
            if (node->right) {
                enter_scope(table);
                valid &= check_statement(node->right, table);
                exit_scope(table);
            } else {
                // if statement must have a body
                semantic_error(SEM_ERROR_INVALID_OPERATION, "if statement", node->token.line);
                valid = 0;
            }
            break;
        }
    
        case AST_WHILE: {
            // validate condition
            valid &= check_condition(node->left, table);
            
            // validate loop body
            if (node->right) {
                enter_scope(table);
                valid &= check_statement(node->right, table);
                exit_scope(table);
            } else {
                // while loop must have a body
                semantic_error(SEM_ERROR_INVALID_OPERATION, "while loop", node->token.line);
                valid = 0;
            }
            break;
        }
    
        case AST_REPEAT: {
            // validate loop body first
            if (node->left) {
                enter_scope(table);
                valid &= check_statement(node->left, table);
                exit_scope(table);
            } else {
                // repeat loop must have a body
                semantic_error(SEM_ERROR_INVALID_OPERATION, "repeat loop", node->token.line);
                valid = 0;
            }

            // then validate condition
            valid &= check_condition(node->right, table);
            break;
        }
    
        case AST_FUNCTIONDECL:
            // validate function declaration
            valid &= (check_function_call(node, table) != -1);
            break;
    
        default:
            // recursively check left and right children
            valid &= check_statement(node->left, table);
            valid &= check_statement(node->right, table);
            break;
    }
    
    return valid;
}

// semantic analysis function
int analyze_semantics(ASTNode* ast) {
    SymbolTable* table = init_symbol_table();
    int result = check_statement(ast, table);
    free_symbol_table(table);
    return result;
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
