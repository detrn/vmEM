#ifndef LEXER_H 
#define LEXER_H
#include "token.h"
#include <stdio.h>

/*
 * Lexer is the module that interprets and tokenizes the "source_data". 
 * Source_data is a NULL terminated string. Source_data is not modified 
 * by the lexer. Index represents the position of the cursor at the moment.
 * Similarly, column and line represents the line and column where the index is 
 * in the source file
 */

typedef struct{
    const char* source_data;
    size_t index; 
    size_t column;
    size_t line; 
}Lexer;

/*
 * Lexer initi intializez the lexer with:
 * .column = 1 
 * .line =1 
 * .index = 0
 * .source_data is assigned the source provided by the source 
 * pointer which has to point to a null character string.  
 */

void lexer_init(Lexer* lexer, const char* source);

/*
 * The function returns the next token available from the source file. 
 * In case of succes the token will have its type set with one of the 
 * types from the token_type enum except token invalid which is used 
 * to signal an error. Token.data corresponds only with the identifier type. 
 * In the rest of the cases the value will be initialized. 
 * The end of the file is signaled with the token type token_eof 
 */
Token next_token(Lexer* lexer);

/*
 * The function is used by defining the DEBUG_LEXER_PRINT macro. It displays 
 * useful information about the lexers status such as the index, column, line
 * and current token 
 */

void lexer_debug(const Lexer* lexer,const Token* token);

#if defined  DEBUG_LEXER || defined DEBUG 
    #define DEBUG_LEXER_PRINT(lexer,token) lexer_debug(lexer, token)
    
#else 
    #define DEBUG_LEXER_PRINT(lexer,token)((void)0)
#endif 
#endif 
