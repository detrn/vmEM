#ifndef  TOKEN_H
#define TOKEN_H 

/*
 * lexeme = {token_name, atrb_val} (input -> lexer -> lexeme)
 */
#include <stdint.h>
#include <stddef.h>
typedef enum{
    REG_RAX,
    REG_RCX,
    REG_RDX,
    REG_RBX,
    REG_RSP,
    REG_RBP,
    REG_RSI,
    REG_RDI,
    REG_R8,
    REG_R9,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14
} Registrii_t;
typedef enum{ 
    token_instruction,
    token_immediate,
    token_comma,
    token_semicolon,
    token_colon,
    token_eof,
    token_invalid,
    token_identifier,
    token_newline,
    token_registru
}Token_type_t ;

typedef struct{
    const char* p; 
    size_t size; 
}String; 

typedef struct{ 
    Token_type_t type; 
    union{
	String data; 
	uint16_t valoare; 
	Registrii_t registru;
    };
}Token;
#endif 
