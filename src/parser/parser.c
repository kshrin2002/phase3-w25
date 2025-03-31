/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"
#include <string.h> // for strcmp

// TODO 1: Add more parsing function declarations for:
// - if statements: if (condition) { ... }
// - while loops: while (condition) { ... }
// - repeat-until: repeat { ... } until (condition)
// - print statements: print x;
// - blocks: { statement1; statement2; }
// - factorial function: factorial(x)

static ASTNode* parse_while_statement(void);
static ASTNode* parse_repeat_statement(void);
static ASTNode* parse_print_statement(void);
static ASTNode* parse_block(void);
static ASTNode* parse_factorial(void);

static ASTNode *parse_statement(void);
static ASTNode* parse_expression(void);
// End of added

// Current token being processed
static Token current_token;
static int position = 0;
static const char *source;

static void parse_error(ParseError error, Token token) {
    // TODO 2: Add more error types for: - done
    // - Missing parentheses
    // - Missing condition
    // - Missing block braces
    // - Invalid operator
    // - Function call errors

    printf("Parse Error at line %d: & Column %d: \n", token.line, token.column);
    switch (error) {
        case PARSE_ERROR_UNEXPECTED_TOKEN:
            printf("Unexpected token '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_SEMICOLON:
            printf("Missing semicolon after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_IDENTIFIER:
            printf("Expected identifier after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_EQUALS:
            printf("Expected '=' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_INVALID_EXPRESSION:
            printf("Invalid expression after '%s'\n", token.lexeme);
            break;

        // Part of TODO 2: -dharsan (expand)
        case PARSE_ERROR_MISSING_L_PAREN:
            printf("Missing opening parenthesis after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_R_PAREN:
            printf("Missing closing parenthesis for expression starting with '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_CONDITION:
            printf("Missing condition after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_L_BRACE:
            printf("Missing opening brace '{' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_R_BRACE:
            printf("Missing closing brace '}' for block starting with '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_INVALID_OPERATOR:
            printf("Invalid operator '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_FUNCTION_CALL_NO_ARGUMENTS:
            printf("Function '%s' called with no arguments but requires some\n", token.lexeme);
            break;
        case PARSE_ERROR_FUNCTION_CALL_INVALID_ARGUMENT:
            printf("Invalid argument in call to function '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_FUNCTION_CALL_TOO_MANY_ARGUMENTS:
            printf("Too many arguments in call to function '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_FUNCTION_UNDEFINED:
            printf("Call to undefined function '%s'\n", token.lexeme);
            break;
        default:
            printf("Unknown error\n");
    }
}

// Get next token
static void advance(void) {
    printf("%s\n", current_token.lexeme);
    current_token = get_next_token(source, &position);
}

// Create a new AST node
static ASTNode *create_node(ASTNodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node) {
        node->type = type;
        node->token = current_token;
        node->left = NULL;
        node->right = NULL;
        node->args = NULL;
    }
    return node;
}

// Match current token with expected type
static int match(TokenType type) {
    return current_token.type == type;
}

// Expect a token type or error
static void expect(TokenType type) {
    if (match(type)) {
        advance();
    } else {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        exit(1); // Or implement error recovery
    }
}

// TODO 3: Add parsing functions for each new statement type
// If statement parsing: if (condition) statement -done
static ASTNode *parse_if_statement(void) {
    ASTNode *node = create_node(AST_IF);
    advance(); // consume 'if'
    if (!match(TOKEN_LPAREN)) {
        parse_error(PARSE_ERROR_MISSING_L_PAREN, current_token);
        exit(1);
    }
    advance();
    node->left = parse_expression(); // condition
    if (!match(TOKEN_RPAREN)) {
        parse_error(PARSE_ERROR_MISSING_R_PAREN, current_token);
        exit(1);
    }
    advance();
    node->right = parse_statement(); // statement after if
    return node;
}

// While loop parsing: while (condition) statement -done
static ASTNode* parse_while_statement(void) { 
    ASTNode *node = create_node(AST_WHILE);
    advance(); // consume 'while'
    if (!match(TOKEN_LPAREN)) {
        parse_error(PARSE_ERROR_MISSING_L_PAREN, current_token);
        exit(1);
    }
    advance();
    node->left = parse_expression(); // condition
    if (!match(TOKEN_RPAREN)) {
        parse_error(PARSE_ERROR_MISSING_R_PAREN, current_token);
        exit(1);
    }
    advance();
    node->right = parse_statement(); // loop body
    return node;
}

// Repeat-until loop parsing: repeat statement until (condition) -done
static ASTNode* parse_repeat_statement(void) { 
    ASTNode *node = create_node(AST_REPEAT);
    advance();
    node->left = parse_statement(); 
    if (!match(TOKEN_UNTIL)){
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        exit(1);
    }
    advance();
    if (!match(TOKEN_LPAREN)) {
        parse_error(PARSE_ERROR_MISSING_L_PAREN, current_token);
        exit(1);
    }
    advance();
    node->right = parse_expression(); // condition
    if (!match(TOKEN_RPAREN)) {
        parse_error(PARSE_ERROR_MISSING_R_PAREN, current_token);
        exit(1);
    }
    advance(); // consume ')'
    return node;
}

// Print statement parsing: print expression - done 
static ASTNode* parse_print_statement(void) { 
    ASTNode *node = create_node(AST_PRINT);
    advance();
    node->left = parse_expression(); 
    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Block parsing - done 
static ASTNode* parse_block(void) { 
    ASTNode *block_node = create_node(AST_BLOCK);
    advance(); // consume '{'

    ASTNode *current = NULL; // helpful for chaining statements
    // now, we parse the statements until we hit the closing brace.

    while (!match(TOKEN_RBRACE) && current_token.type != TOKEN_EOF) {
        // single statement parsing
        ASTNode *statement = parse_statement();
        // now if we are looking at the first statement in the block, attach it as the left child.
        if (block_node->left == NULL) {
            block_node->left = statement;
        } else {
            // if we are not looking at the first statement, chain the statements together.
            current->right = statement;
        }
        // update the last statement to the current statement
        current = statement;
    }
    // eat the closing brace
    if (!match(TOKEN_RBRACE)) {
        parse_error(PARSE_ERROR_MISSING_R_BRACE, current_token);
        exit(1);
    }
    advance(); // consume '}'
    return block_node;
}

// Factorial function call parsing
static ASTNode* parse_factorial(void) { 
    ASTNode *node = create_node(AST_FUNCTIONCALL);
    advance(); // consume 'factorial'

    if (!match(TOKEN_LPAREN)) {
        parse_error(PARSE_ERROR_MISSING_L_PAREN, current_token);
        exit(1);
    }
    advance(); // consume '('

    node->args = parse_expression(); // Parse argument
    
    if (!match(TOKEN_RPAREN)) {
        parse_error(PARSE_ERROR_MISSING_R_PAREN, current_token);
        exit(1);
    }
    advance(); // consume ')'
    return node;
}

// Parse variable declaration: int x;
static ASTNode *parse_declaration(void) {
    ASTNode *node = create_node(AST_VARDECL);
    advance(); // consume 'int'

    if (!match(TOKEN_IDENTIFIER)) {
        parse_error(PARSE_ERROR_MISSING_IDENTIFIER, current_token);
        exit(1);
    }

    node->token = current_token;
    advance(); // consume x

    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parse assignment: x = 5;
static ASTNode *parse_assignment(void) {
    ASTNode *node = create_node(AST_ASSIGN);
    node->left = create_node(AST_IDENTIFIER);
    node->left->token = current_token;
    advance();

    if (!match(TOKEN_EQUALS)) {
        parse_error(PARSE_ERROR_MISSING_EQUALS, current_token);
        exit(1);
    }
    advance(); // eat equals

    node->right = parse_expression();

    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parse statement
static ASTNode *parse_statement(void) {
    if (match(TOKEN_KEYWORD)) {
        return parse_declaration();
    } else if (match(TOKEN_IDENTIFIER)) {
        return parse_assignment();
    } else if (match(TOKEN_LBRACE)) {
        return parse_block();
    } else if (match(TOKEN_IF)) {
        return parse_if_statement();
    } else if (match(TOKEN_WHILE)) {
        return parse_while_statement();
    } else if (match(TOKEN_REPEAT)) {
        return parse_repeat_statement();
    } else if (match(TOKEN_PRINT)) {
        return parse_print_statement();
    }

    printf("Syntax Error: Unexpected token\n");
    exit(1);
}

// Parse expression (handles numbers and identifiers)
static ASTNode* parse_expression(void) {
    ASTNode* node = NULL;
    
    // Handle number literals
    if (match(TOKEN_NUMBER)) {
        node = create_node(AST_NUMBER);
        advance(); // consume number
    } 
    // Handle identifiers (variables and function calls)
    else if (match(TOKEN_IDENTIFIER)) {
        // Special case for factorial function
        if (strcmp(current_token.lexeme, "factorial") == 0) {
            return parse_factorial();
        }
        
        node = create_node(AST_IDENTIFIER);
        advance(); // consume identifier
        
        // Check if it's a function call
        if (match(TOKEN_LPAREN)) {
            node->type = AST_FUNCTIONCALL;
            advance(); // consume '('
            
            // Parse arguments if any
            if (!match(TOKEN_RPAREN)) {
                node->args = parse_expression();
            }
            
            if (!match(TOKEN_RPAREN)) {
                parse_error(PARSE_ERROR_MISSING_R_PAREN, current_token);
                exit(1);
            }
            advance(); // consume ')'
        }
    } 
    // Handle parenthesized expressions
    else if (match(TOKEN_LPAREN)) {
        advance(); // consume '('
        node = parse_expression();
        
        if (!match(TOKEN_RPAREN)) {
            parse_error(PARSE_ERROR_MISSING_R_PAREN, current_token);
            exit(1);
        }
        advance(); // consume ')'
    }
    // Handle operators
    else if (match(TOKEN_OPERATOR)) {
        node = create_node(AST_OPERATOR);
        advance(); // consume operator
        node->right = parse_expression();
    }
    else {
        return NULL; // No expression found
    }
    
    // Handle binary operations
    if (match(TOKEN_OPERATOR) && node != NULL) {
        ASTNode* binop = create_node(AST_BINOP);
        binop->left = node;
        binop->token = current_token; // Store the operator
        advance(); // consume operator
        binop->right = parse_expression();
        return binop;
    }
    
    return node;
}

// Parse program (multiple statements)
static ASTNode *parse_program(void) {
    ASTNode *program = create_node(AST_PROGRAM);
    ASTNode *current = program;

    while (!match(TOKEN_EOF)) {
        current->left = parse_statement();
        if (!match(TOKEN_EOF)) {
            current->right = create_node(AST_PROGRAM);
            current = current->right;
        }
    }

    return program;
}

// Initialize parser
void parser_init(const char *input) {
    source = input;
    position = 0;
    advance(); // Get first token
}

// Main parse function
ASTNode *parse(void) {
    return parse_program();
}

// Print AST (for debugging)
void print_ast(ASTNode *node, int level) {
    if (!node) return;

    // Indent based on level
    for (int i = 0; i < level; i++) printf("  ");

    // Print node info
    switch (node->type) {
        case AST_PROGRAM:
            printf("Program\n");
            break;
        case AST_VARDECL:
            printf("VarDecl: %s\n", node->token.lexeme);
            break;
        case AST_ASSIGN:
            printf("Assign\n");
            break;
        case AST_NUMBER:
            printf("Number: %s\n", node->token.lexeme);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->token.lexeme);
            break;
        case AST_FUNCTIONCALL:
            printf("FunctionCall: %s\n", node->token.lexeme);
            break;
        case AST_IF:
            printf("IfStatement: %s\n", node->token.lexeme);
            break;
        case AST_WHILE:
            printf("WhileLoop: %s\n", node->token.lexeme);
            break;
        case AST_REPEAT:
            printf("Repeat: %s\n", node->token.lexeme);
            break;
        case AST_PRINT:
            printf("Print\n");
            break;
        case AST_BLOCK:
            printf("Block\n");
            break;
        case AST_BINOP:
            printf("BinaryOp: %s\n", node->token.lexeme);
            break;
        case AST_COMP:
            printf("Comparison: %s\n", node->token.lexeme);
            break;
        case AST_OPERATOR:
            printf("Operator: %s\n", node->token.lexeme);
            break;
        default:
            printf("Unknown node type\n");
    }

    // Print children
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

// Free AST memory
void free_ast(ASTNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}

// // Main function for testing
// int main() {
//     // Test with both valid and invalid inputs
//     const char *input = "int x;\n" // Valid declaration
//             "x = 42;\n"; // Valid assignment;
//     // // TODO 8: Add more test cases and read from a file:
//     // const char *invalid_input = "int x;\n"
//     //                             "x = 42;\n"
//     //                             "int ;";


//     printf("Parsing input:\n%s\n", input);
//     parser_init(input);
//     ASTNode *ast;
//     ast = parse();
//     printf("\nAbstract Syntax Tree:\n");
//     print_ast(ast, 0);
//     free_ast(ast);

//     return 0;
// }