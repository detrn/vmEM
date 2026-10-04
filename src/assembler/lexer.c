#include "assembler/lexer.h"
#include "assembler/token.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef INSTRUCTION_NUMBER
#define INSTRUCTION_NUMBER 17
#endif 
#ifndef REG_NUMBER
#define REG_NUMBER 15
#endif 

void lexer_init(Lexer *lexer, const char *source){
    lexer->source_data = source;
    lexer->column = 1;
    lexer->line = 1;
    lexer->index = 0; 
}
/*
void nextChar(Lexer* lexer){
     if(lexer->source_data[lexer->index + 1] =='\0') 

}
*/
const char *instructions[INSTRUCTION_NUMBER] = {"HALT", "NOP", "RRMOVW", "IRMOVW", "RMMOVW", "MRMOVW", "ADD", "SUBW", "ANDW", "XORW", "JMP", "JLE", "JL", "JE", "JNE", "JGE", "JG"};

const char* registrii[15] = {"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", "R8", "R9", "R10", "R11", "R12", "R13", "R14"};
static void advance(Lexer* lexer);
static Token scan_for_register(Lexer* lexer){
    size_t start = lexer->index; 
    char c = lexer->source_data[lexer->index];
    while(isalnum((unsigned char)c)){
	advance(lexer);
	c= lexer->source_data[lexer->index];
    }
    size_t end = lexer->index;
    if( start == end){
	Token token = {.type = token_invalid, .valoare = 0};
	return token; 
    }
    size_t str_len = end - start;
    String string = {.p = &lexer->source_data[start], .size = end - start};
    for(int i = 0 ; i < REG_NUMBER; i++){
	if(str_len ==strlen(registrii[i]) && !memcmp(string.p,registrii[i],end-start)){
	    Token token ={.type = token_registru, .data = string};
	    return token;
	}
    }
    Token token ={.type=token_invalid, .valoare = 0};
    return token;
    
}
static Token scan_for_word(Lexer* lexer){
    size_t start = lexer->index;
    char c = lexer->source_data[lexer->index];
    while(isalnum((unsigned char)c) || c =='_'){
	advance(lexer);
	c = lexer->source_data[lexer->index]; 
    }
    size_t end = lexer->index;
    String string ={ .p = &lexer->source_data[start], .size = end - start};

    //Caz 1: e instructiune
    for(int i = 0 ; i < INSTRUCTION_NUMBER; i++){
	size_t instr_size = strlen(instructions[i]);
	if(string.size == instr_size && !memcmp(string.p,instructions[i],end-start)){
	    Token token ={.type = token_instruction, .data = string};
	    return token;
	}
    }
    //Caz 2: secventa identifier 
    Token token ={.type = token_identifier, .data = string};
	return token;
}
static Token scan_for_number(Lexer* lexer){
    char c = lexer->source_data[lexer->index];
    size_t start = lexer->index; 
    while(isdigit((unsigned char)c)){
	advance(lexer);
	c = lexer->source_data[lexer->index];
    }
    size_t end = lexer->index; 
    if(start == end){ 
    	Token token = {.type = token_invalid, .valoare = 0};
	return token; 

    }
    char buffer[32];
    if( end - start >= sizeof(buffer)){ 
	Token token = {.type = token_invalid, .valoare = 0};
	return token; 
    }
    memcpy(buffer,&lexer->source_data[start],end-start);
    buffer[end-start ] = '\0';
    int valoare = atoi(buffer);
    if(valoare > 65535) return (Token){.type = token_invalid, .valoare = 0};
    Token token ={.type=token_immediate,.valoare =valoare};
    return token;
}

static void advance(Lexer* lexer){
    char c = lexer->source_data[lexer->index];
    lexer->index++;
    if(c =='\n'){
	lexer->line++;
	lexer->column = 1;
    }
    else lexer->column++;
}
static void skip_comment(Lexer* lexer){
    while(lexer->source_data[lexer->index]!='\n' &&lexer->source_data[lexer->index]!='\0') advance(lexer);
}
Token next_token(Lexer* lexer){
    while(1){
	char c = lexer->source_data[lexer->index];
	if(c =='\0'){
	    Token tokenEof ={ .type = token_eof, .valoare = 0};
	    return tokenEof;
	}
	if(c == ' ' || c == '\t') {
	    advance(lexer);
	    c = lexer->source_data[lexer->index];
	    continue;
	}
	if(c =='\n'){
	    advance(lexer);
	    Token token_newLine ={.type = token_newline, .valoare = 0};
	    return token_newLine;
	}
	if(isalpha((unsigned char)c)) return scan_for_word(lexer); //ptc lexer are tot ce i trebuie e baiat mare
	if(c == '$') {
	    advance(lexer);
	    return scan_for_number(lexer);
	}
	if(c == '%'){
	    advance(lexer);
	    return scan_for_register(lexer);
	}
	if( c==','){
	    advance(lexer);
	    Token token ={ .type = token_comma, .valoare = 0};
	    return token;
	}
	if( c==';'){
	   skip_comment(lexer); 
	   continue;
	}
	if( c==':'){
	    advance(lexer);
	    Token token = {.type = token_colon, .valoare =0};
	    return token;
	}
	advance(lexer);
	Token token = {.type =token_invalid, .valoare = 0};
	return token;
	/*
	 * TO DO : MACROS si DIRECTIVE in limita bunului simt 
	 */
	/* if( c== '.') return scar_for_directive(lexer); TO DO */  
	 //nu mai incrementez index ptc oricum nu vreau sa mai citesc 
    }
}
