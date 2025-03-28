/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

// Basic node types for AST
typedef enum {
    AST_PROGRAM,        // Program node
    AST_VARDECL,        // Variable declaration (int x)
    AST_ASSIGN,         // Assignment (x = 5)
    AST_PRINT,          // Print statement
    AST_NUMBER,         // Number literal
    AST_IDENTIFIER,     // Variable name
    // TODO: Add more node types as needed
    // Added by Shrinidhi
    AST_IF,             // If statement
    AST_WHILE,          // While loop       
    AST_REPEAT,         // Repeat until loop
    AST_BLOCK,          // Block statements
    AST_FUNCTIONCALL,   // Function call for factorial(x)
    AST_FUNCTIONDECL,
    // End of added
    // added new node types as used in to do 6 - dharsan
    AST_BINOP,
    // Added by Lucy
    AST_COMP,
    AST_OPERATOR,
} ASTNodeType;

typedef enum {
    PARSE_ERROR_NONE,
    PARSE_ERROR_UNEXPECTED_TOKEN,
    PARSE_ERROR_MISSING_SEMICOLON,
    PARSE_ERROR_MISSING_IDENTIFIER,
    PARSE_ERROR_MISSING_EQUALS,
    PARSE_ERROR_INVALID_EXPRESSION,
    // Part of To DO 2: -dharsan
    PARSE_ERROR_MISSING_L_PAREN,
    PARSE_ERROR_MISSING_R_PAREN,
    PARSE_ERROR_MISSING_CONDITION,
    PARSE_ERROR_MISSING_L_BRACE,
    PARSE_ERROR_MISSING_R_BRACE,
    PARSE_ERROR_INVALID_OPERATOR,
    PARSE_ERROR_FUNCTION_CALL_NO_ARGUMENTS,
    PARSE_ERROR_FUNCTION_CALL_INVALID_ARGUMENT,
    PARSE_ERROR_FUNCTION_CALL_TOO_MANY_ARGUMENTS,
    PARSE_ERROR_FUNCTION_UNDEFINED
} ParseError;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;           // Type of node
    Token token;               // Token associated with this node
    struct ASTNode* left;      // Left child
    struct ASTNode* right;     // Right child
    // TODO: Add more fields if needed
    struct ASTNode* args;
} ASTNode;

// Parser functions
void parser_init(const char* input);
ASTNode* parse(void);
void print_ast(ASTNode* node, int level);
void free_ast(ASTNode* node);

#endif /* PARSER_H */