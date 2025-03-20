/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

/* Token types that need to be recognized by the lexer
 * TODO: Add more token types as per requirements:
 * - Keywords or reserved words (if, repeat, until)
 * - Identifiers
 * - String literals
 * - More operators
 * - Delimiters
 */
typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER,     // e.g., "123", "456"
    TOKEN_OPERATOR,   // e.g., "+", "-"
    // added by Lucy
    TOKEN_IDENTIFIER, // variable names like "x", "varName"
    TOKEN_ASSIGN,     // assignment operator "="
    TOKEN_KEYWORD,    // keywords like "if", "repeat"
    TOKEN_STRING,     // string literals like "hello", "world"
    TOKEN_DELIMITER,  // delimiters like ",", ";", "{", "}", "(", ")"
    TOKEN_COMMENT,    // comments like "// comment", "/* block comment */"
    // end of added
    TOKEN_ERROR
} TokenType;

/* Error types for lexical analysis
 * TODO: Add more error types as needed for your language - as much as you like !!
 */
typedef enum {
    ERROR_NONE,
    ERROR_INVALID_CHAR,
    ERROR_INVALID_NUMBER,
    ERROR_CONSECUTIVE_OPERATORS,
    // added by Lucy
    ERROR_UNTERMINATED_STRING,
    ERROR_INVALID_IDENTIFIER,
    // Yash
    ERROR_UNTERMINATED_COMMENT, // user forgets to close comments with */
    // Dharsan
    ERROR_STRING_BUFFER_OVERFLOW,


} ErrorType;

/* Token structure to store token information
 * TODO: Add more fields if needed for your implementation
 * Hint: You might want to consider adding line and column tracking if you want to debug your lexer properly.
 * Don't forget to update the token fields in lexer.c as well
 */
typedef struct {
    TokenType type;
    char lexeme[100];   // Actual text of the token
    int line;           // Line number in source file
    ErrorType error;    // Error type if any
} Token;

#endif /* TOKENS_H */