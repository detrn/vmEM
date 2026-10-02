#include "../../include/assembler/lexer.h"
#include "../../include/assembler/token.h"
#include <ctype.h>
#include <stdio.h>


void lexer_init(Lexer *lexer, const char *source){
    lexer->source_data = source;
    lexer->column = 1;
    lexer->line = 1;
    lexer->index = 0; 
}

static void advance(Lexer* lexer){
    lexer->column++;
    char c = lexer->source_data[lexer->index];
    if(c =='\n'){
	lexer->line++;
	lexer->column = 1; 
    }
    else lexer->index++; 
}

static Token scan_for_letter(Lexer* lexer){
    char c = lexer->source_data[lexer->index];
    size_t start = lexer->index; 
    while(isalpha(c)){
	advance(lexer);
	c = lexer->source_data[lexer->index];
    }
    size_t end = lexer->index; 

    return token;
}

Token next_token(Lexer* lexer){
    char c = lexer->source_data[lexer->index];
    while(c!= '\0'){
	if(c == ' ' || c == '\t') {
	    advance(lexer);
	    continue;
	}
	if(c =='\n'){
	    Token token_newLine ={.type = token_newline, .valoare = 0};
	    return token_newLine;
	}
	if(c >= 'A' && c<='z') return scan_for_letter(lexer); //ptc lexer are tot ce i trebuie e baiat mare
	if(c == '$') return scan_for_number(lexer);
	if(c == '%') return scan_for_register(lexer);  
	/* if( c== '.') return scar_for_directive(lexer); TO DO */  
	Token token_error ={ .type = token_invalid, .valoare = 0}; //nu mai incrementez index ptc oricum nu vreau sa mai citesc 
	return token_error;
    }
}
