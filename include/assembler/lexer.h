#ifndef LEXER_H 
#define LEXER_H
#include "token.h"
#include <stdio.h>

typedef struct{
    const char* source_data;
    size_t index; 
    size_t column;
    size_t line; 
}Lexer;

void lexer_init(Lexer* lexer, const char* source);
Token next_token(Lexer* lexer);


#endif 
