#ifndef PARSER_H 
#define PARSER_H 
#include "lexer.h"
#include "token.h"
#include "statement.h"

typedef struct{
    Lexer* lex; 
    Token current; 
    Token next;
    int err; 
}Parser;

Statement parse(Parser* parser);
void parser_init(Parser* parser, Lexer* lexer);
Statement parser_debug_next(Parser* parser); 

#if defined DEBUG_PARSER || DEBUG 
    #define DEBUG_PARSER_PRINT(parser) parser_debug_next(parser)
    
#else 
    #define DEBUG_PARSER_PRINT(parser) ((void)0)
#endif
#endif 


