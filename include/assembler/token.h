#ifndef  TOKEN_H
#define TOKEN_H 

/*
 * lexeme = {token_name, atrb_val} (input -> lexer -> lexeme)
 */
#include <stdint.h>
#include <stddef.h>

/*
 * Token type represents the type of lexemes that the lexer can recognise.
 * The tokens's type is further used in the other modules for processing. 
 */

typedef enum{ 
    token_instruction, //corespunde cu valoare 
    token_number,//1  
    token_dollar,//2  
    token_rbracket,//3
    token_lbracket,//4 
    token_comma,//5 
    token_semicolon,//6 
    token_colon,//7 
    token_eof,//8 
    token_invalid,//9 
    token_identifier,//corespunde cu data 10 
    token_newline, //11
    token_registru //corespunde cu valoare 12
}Token_type;

/*
 * String is used for labels and other string immediates. 
 * For flexibility and ease of use it is not guaranteed that it isNULL terminated.
 */

typedef struct{
    const char* p; 
    size_t size; 
}String; 

/*
 * Column and line are used for error reporting. If there is an error in any of the processing stages 
 * these can be used to display exact information about where the error is. 
 */
typedef struct{ 
    Token_type type; 
    union{
	String data; 
	uint16_t valoare; 
    };
    size_t column, line;
}Token;
#endif 
