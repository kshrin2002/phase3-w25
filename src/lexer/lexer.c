
/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"

// Line tracking
static int current_line = 1;
static char last_token_type = 'x'; // For checking consecutive operators

/* Print error messages for lexical errors */
void print_error(ErrorType error, int line, const char *lexeme) {
    printf("Lexical Error at line %d: ", line);
    switch (error) {
        case ERROR_INVALID_CHAR:
            printf("Invalid character '%s'\n", lexeme);
            break;
        case ERROR_INVALID_NUMBER:
            printf("Invalid number format\n");
            break;
        case ERROR_CONSECUTIVE_OPERATORS:
            printf("Consecutive operators not allowed\n");
            break;
        case ERROR_UNTERMINATED_COMMENT:
            printf("Unterminated multi line comment, check EOL\n");
            break;

        // error cases added by - Dharsan R
        case ERROR_UNTERMINATED_STRING:
            printf("Unterminated string, check EOL\n");
            break;

        case ERROR_STRING_BUFFER_OVERFLOW:
            printf("String too long, buffer overflow reached! Make string shorter or split.\n");
            break;

        case ERROR_INVALID_IDENTIFIER:
            printf("Invalid identifier format\n");
            break;

        default:
            printf("Unknown error\n");
    }
}

/* Print token information
 *
 *  TODO Update your printing function accordingly
 */

void print_token(Token token) {
    if (token.error != ERROR_NONE) {
        print_error(token.error, token.line, token.lexeme);
        return;
    }

    printf("Token: ");
    switch (token.type) {
        case TOKEN_NUMBER:
            printf("NUMBER");
            break;
        case TOKEN_OPERATOR:
            printf("OPERATOR");
            break;
        case TOKEN_EOF:
            printf("EOF");
            break;
        // print cases - Dharsan R
        case TOKEN_IDENTIFIER:
            printf("IDENTIFIER");
            break;
        case TOKEN_ASSIGN:
            printf("ASSIGN");
            break;
        case TOKEN_KEYWORD:
            printf("KEYWORD");
            break;
        case TOKEN_STRING:
            printf("STRING");
            break;
        case TOKEN_DELIMITER:
            printf("DELIMITER");
            break;
        case TOKEN_COMMENT:
            printf("COMMENT");
            break;

        default:
            printf("UNKNOWN");
    }
    printf(" | Lexeme: '%s' | Line: %d\n",
           token.lexeme, token.line);
}

/* Get next token from input */
Token get_next_token(const char *input, int *pos) {
    Token token = {TOKEN_ERROR, "", current_line, ERROR_NONE};
    char c;

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && (c == ' ' || c == '\n' || c == '\t')) {
        if (c == '\n') {
            current_line++;
        }
        (*pos)++;
    }

    if (input[*pos] == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    c = input[*pos];

    // TODO: Add comment handling here
    // Added by Lucy
    // Handle comments
    if (c == '/' && input[*pos + 1] == '/') {
        while (input[*pos] != '\n' && input[*pos] != '\0') {
            (*pos)++; // skip everything in the comment
        }
        return get_next_token(input, pos); // get next token
    }// Block comment handling by yash 

    else if (c == '/' && input [*pos + 1] == '*') { // start of a block comment, edge case noted- ensure only enter block when you have sequence /*.
        (*pos) += 2; // skip the opening comment
        int comment_check = 0; // here we can simply make a flag to check if the comment is closed or not

        // keep going until we reach the end of comment
        while (input[*pos] != '\0') {
            if (input[*pos] == '\n') {
                current_line++;
            }

            if (input[*pos] == '*' && input[*pos + 1] == '/') {
                (*pos) += 2; // skip the closing comment
                comment_check = 1;
                break;
            }
            (*pos)++;
        }
        if (comment_check == 0) {
            token.error = ERROR_UNTERMINATED_COMMENT;
            return token;
            }
        return get_next_token(input, pos); // grab next token after the comment
    }
    


    // Handle numbers
    if (isdigit(c)) {
        int i = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while (isdigit(c) && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // TODO: Add keyword and identifier handling here
    // Hint: You'll have to add support for keywords and identifiers, and then string literals
    // Added by Lucy
    // Handle keywords and identifiers
    if (isalpha(c) || c == '_') { // variable names start with a letter or _
        int i = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while ((isalnum(c) || c == '_') && i < sizeof(token.lexeme) - 1);
        token.lexeme[i] = '\0';
        // check for keyword
        if (strcmp(token.lexeme, "if") == 0 ||
            strcmp(token.lexeme, "int") == 0 ||
            strcmp(token.lexeme, "repeat") == 0 ||
            strcmp(token.lexeme, "until") == 0) {
            token.type = TOKEN_KEYWORD;
            } else {
                token.type = TOKEN_IDENTIFIER;
            }
        return token;
    }


    // TODO: Add string literal handling here
    // Added by Dharsan
    // Handle string literals
    if (c == '"') {
        // used to check if string closed
        int close_check = 0;

        int i = 0;
        // add and skip the first quote character
        token.lexeme[i++] = c;
        (*pos)++;
        c = input[*pos];

        // run until last quote or end of input reached adding a character each loop
        while (c != '"' && c != '\0' && i < sizeof(token.lexeme) - 1) {

            // adds handling where string has escape sequences include them within the quotes
            // helps overcome cases where the a quotation is a part of the string
            if (c=='\\'){
                token.lexeme[i++] = c;
                (*pos)++;
                c = input[*pos];
            }

            // add character and increement pointer 
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        }

        // if size is exceeded and string has not been terminated then return a buffer overlflow
        if (i == sizeof(token.lexeme) - 1 && c!='"'){
            // throws an error because the string is too long
            token.error = ERROR_STRING_BUFFER_OVERFLOW;


            // we still want to check if the string has been terminated and also remove the next bit of
            // characters it allows tokenization of the part that is out of the string
            while (input[*pos]!= '\0'){
                // includes the last quote if it exists if it doesn't still throws buffer error until
                // it is fixes/made shorter then the termination error can take over
                if (input[*pos] == '"') {
                    (*pos) += 1; // skip the closing comment
                    break;
                    close_check = 1;
                }
                (*pos)++;
            }
            // overrides buffer error as string actually has not been terminated and too long
            if (close_check==0){
                token.error = ERROR_UNTERMINATED_STRING;
            }

            return token;
        }

        // if the string is not terminated return an error
        if (c != '"') {
            token.error = ERROR_UNTERMINATED_STRING;
            return token;
        }

        // add the last quote
        token.lexeme[i++] = c;
        (*pos)++;
        c = input[*pos];

        // closes the string
        token.lexeme[i] = '\0';

        token.type = TOKEN_STRING;
        return token;
    }


    // Handle operators 
    if (c == '+' || c == '-' || c == '*' || c=='/' || c == '%') { // *, /, % added by Lucy
        // if (last_token_type == 'o') {
        if (input[*pos + 1] == c) {
            // Check for consecutive operators
            token.error = ERROR_CONSECUTIVE_OPERATORS;
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            (*pos)+=2;
            return token;
        }
        token.type = TOKEN_OPERATOR;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'o';
        (*pos)++;
        return token;
    }

    // Added by Lucy
    // Handle assignment and comparison operators
    if (c == '=') {
        token.type = TOKEN_ASSIGN;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        (*pos)++;

        // Check for "==" (comparison operator)
        if (input[*pos] == '=') {
            token.type = TOKEN_OPERATOR;
            token.lexeme[1] = '=';
            token.lexeme[2] = '\0';
            (*pos)++;
        }

        return token;
    }


    // TODO: Add delimiter handling here
    // Added by Lucy
    // Handle delimiters
    if (strchr(";(),{}", c)) {
        token.type = TOKEN_DELIMITER;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        (*pos)++;
        return token;
    }


    // Handle invalid characters
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    (*pos)++;
    return token;
}

// This is a basic lexer that handles numbers (e.g., "123", "456"), basic operators (+ and -), consecutive operator errors, whitespace and newlines, with simple line tracking for error reporting.

int main() {
    int position = 0;
    Token token;

    // These test cases allow us to input muliple token and match with expected output
    // Additional test cases added by Shrinidhi
    const char *inputs[] = {
        "123 + 456 - 789",
        "int x = 42; y = x + 10;",
        "123 ++ 456",
        "x@ = 10;",
        "\"Hello\"",
        "/* comment */ x = 10; // line",
        "123 + 456 - 789\n1 ++ 2",
        "\"Lorem ipsum dolor sit amet, \nconsectetur adipiscing elit. Sed nunc orci, interdum at imperdiet ut, fringilla\"",
        "\"This vallidates\tspacing escape\nsequences\"",
        "\"String with \\\"quotes\\\"\"",
        "\"Unterminated string",
        "/* Unterminated comment\n"
    };

    const char *expected_outputs[] = {
        "NUMBER 123, OPERATOR +, NUMBER 456, OPERATOR -, NUMBER 789",
        "KEYWORD int, IDENTIFIER x, OPERATOR =, NUMBER 42, DELIMITER ;, IDENTIFIER y, OPERATOR =, IDENTIFIER x, OPERATOR +, NUMBER 10, DELIMITER ;",
        "NUMBER 123, ERROR ++",
        "IDENTIFIER x, ERROR @, OPERATOR =, NUMBER 10, DELIMITER ;",
        "STRING \"Hello\"",
        "IDENTIFIER x, OPERATOR =, NUMBER 10, DELIMITER ;",
        "NUMBER 123, OPERATOR +, NUMBER 456, OPERATOR -, NUMBER 789, NUMBER 1, ERROR ++, NUMBER 2",
        "ERROR String too Long",
        "New Line operator",
        "STRING \"String with \\\"quotes\\\"\"",
        "ERROR Unterminated string",
        "ERROR Unterminated comment"
        
    };

    //end of test cases

    int num_tests = sizeof(inputs) / sizeof(inputs[0]);

    for (int i = 0; i < num_tests; i++) {
        position = 0;
        printf("\nTesting Input: \"%s\"\n", inputs[i]);

        do {
            token = get_next_token(inputs[i], &position);
            print_token(token);

        } while (token.type != TOKEN_EOF);

        printf("\nExpected Output: %s\n", expected_outputs[i]);

        printf("---------------------------------------------\n");
    }

    return 0;

}