#ifndef PARSER_H 
#define PARSER_H 

typedef struct{
    Lexer* lex; 
    Token current; 
    Token next;
    int err; 
}Parser;

void parse(Parser *);

#endif 


