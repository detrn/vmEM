#include "assembler/lexer.h"
#include "assembler/token.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef INSTRUCTION_NUMBER
#define INSTRUCTION_NUMBER 27
#endif
#ifndef REG_NUMBER
#define REG_NUMBER 15
#endif


void lexer_debug(const Lexer* lexer,const Token* token){
    char *types[13] = {"token_instruction", "token_number", "token_dollar", "token_rbracket", "token_lbracket", "token_comma", "token_semicolon", "token_colon", "token_eof", "token_invalid", "token_identifier", "token_newline", "token_registru"};
    //Print lexer 
    printf("Actual index: %zu, actual column: %zu, actual line: %zu\n",lexer->index,lexer->column,lexer->line); 
    //Print token
    printf("Current token info:\n");
    for(int i = 0 ; i < 13; i++){
	if(token->type ==(Token_type)i){
	    printf("Tip:%s\n",types[i]);
	}
    }
    if(token->type == token_identifier) printf("Data: %.*s\n",(int)token->data.size,token->data.p);
    else printf("Valoare: %u\n",token->valoare);
    printf("Coloana: %zu, linie: %zu\n", token->column, token->line);
}

void lexer_init(Lexer *lexer, const char *source){
    lexer->source_data = source;
    lexer->column = 1;
    lexer->line = 1;
    lexer->index = 0;
}
/*
 * Instructions and registrii are used for comparing the source_data labels
 * to the actual enum values. The elements are placed in the order that resembles 
 * the enum ones
 */
const char *instructions[INSTRUCTION_NUMBER] = {
    "HALT", "NOP", "RRMOVW", "IRMOVW", "RMMOVW", "MRMOVW", "ADDW",
    "SUBW", "ANDW", "XORW", "JMP", "JLE", "JL", "JE", "JNE", "JGE", "JG","CMOVLE","CMOVL","CMOVE","CMOVNE","CMOVGE","CMOVG","CALL","RET","PUSHW","POPW"
};

const char *registrii[REG_NUMBER] = {
    "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
    "R8", "R9", "R10", "R11", "R12", "R13", "R14"
};

static void advance(Lexer *lexer);


/*
 * The function is dependent on the registrii string array 
 */ 

static Token scan_for_register(Lexer *lexer){
    size_t start_line = lexer->line;
    size_t start_column = lexer->column;
    advance(lexer);
    size_t start = lexer->index;
    char c = lexer->source_data[lexer->index];
    while(isalnum((unsigned char)c)){
        advance(lexer);
        c = lexer->source_data[lexer->index];
    }
    size_t end = lexer->index;
    if(start == end){
        Token token = {.type = token_invalid, .valoare = 0, .column = start_column, .line = start_line};
        return token;
    }
    size_t str_len = end - start;
    String string = {.p = &lexer->source_data[start], .size = end - start};
    for(size_t i = 0; i < REG_NUMBER; i++){
        if(str_len == strlen(registrii[i]) && !memcmp(string.p, registrii[i], end-start)){
            Token token = {.type = token_registru, .valoare =(uint16_t) i, .column = start_column, .line = start_line}; //FOARTE IMPORTANT: registrii devine SFANT. El e ordinea suprema si altfel nu merge programul. 
            return token;
        }
    }
    Token token = {.type = token_invalid, .valoare = 0, .column = start_column, .line = start_line};
    return token;
}

/*
 * It's the same as it is with the function before, but 
 * with the instrucions array 
 */

static Token scan_for_word(Lexer *lexer){
    size_t start_line = lexer->line;
    size_t start_column = lexer->column;
    size_t start = lexer->index;
    char c = lexer->source_data[lexer->index];
    while(isalnum((unsigned char)c) || c == '_'){
        advance(lexer);
        c = lexer->source_data[lexer->index];
    }
    size_t end = lexer->index;
    String string = {.p = &lexer->source_data[start], .size = end - start};
    // Caz 1: e instructiune
    for(size_t i = 0; i < INSTRUCTION_NUMBER; i++){
        size_t instr_size = strlen(instructions[i]);
        if(string.size == instr_size && !memcmp(string.p, instructions[i], end-start)){
            Token token = {.type = token_instruction, .valoare = (uint16_t)i, .column = start_column, .line = start_line}; //FOARTE IMPORTANT: e la fel ca la scan for register. 
														//Iau stringul si l convertesc la index (ca sa am informatia pregatite 
														//pentru enums din parser)
            return token;
        }
    }
    // Caz 2: secventa identifier
    Token token = {.type = token_identifier, .data = string, .column = start_column, .line = start_line};
    return token;
}

static Token scan_for_number(Lexer *lexer){
    size_t start_line = lexer->line;
    size_t start_column = lexer->column;
    char c = lexer->source_data[lexer->index];
    size_t start = lexer->index;
    while(isdigit((unsigned char)c)){
        advance(lexer);
        c = lexer->source_data[lexer->index];
    }
    size_t end = lexer->index;
    if(start == end){
        Token token = {.type = token_invalid, .valoare = 0, .column = start_column, .line = start_line};
        return token;
    }
    char buffer[32];
    if(end - start >= sizeof(buffer)){
        Token token = {.type = token_invalid, .valoare = 0, .column = start_column, .line = start_line};
        return token;
    }
    memcpy(buffer, &lexer->source_data[start], end-start);
    buffer[end-start] = '\0';
    int valoare = atoi(buffer);
    if(valoare > 65535) return (Token){.type = token_invalid, .valoare = 0, .column = start_column, .line = start_line};
    Token token = {.type = token_number, .valoare = (uint16_t)valoare, .column = start_column, .line = start_line};
    return token;
}

/*
 * The function advances the cursor one character at a time. 
 * It modifies the informative values and resets column on new line 
 */

static void advance(Lexer *lexer){
    char c = lexer->source_data[lexer->index];
    lexer->index++;
    if(c == '\n'){
        lexer->line++;
        lexer->column = 1;
    }
    else lexer->column++;
}

static void skip_comment(Lexer *lexer){
    while(lexer->source_data[lexer->index] != '\n' && lexer->source_data[lexer->index] != '\0') advance(lexer);
}
Token next_token(Lexer *lexer){
    while(1){
        char c = lexer->source_data[lexer->index];
        if(c == '\0'){
            Token tokenEof = {.type = token_eof, .valoare = 0, .column = lexer->column, .line = lexer->line};
            return tokenEof;
        }
        if(c == ' ' || c == '\t'){
            advance(lexer);
            c = lexer->source_data[lexer->index];
            continue;
        }
        if(c == '\n'){
            Token token_newLine = {.type = token_newline, .valoare = 0, .column = lexer->column, .line = lexer->line};
            advance(lexer);
            return token_newLine;
        }
        if(isalpha((unsigned char)c)) return scan_for_word(lexer);
        if(isdigit((unsigned char)c)) return scan_for_number(lexer);
        if(c == '$'){
            Token token = {.type = token_dollar, .valoare = 0, .column = lexer->column, .line = lexer->line};
            advance(lexer);
            return token;
        }
        if(c == '%') return scan_for_register(lexer);
        if(c == ','){
            Token token = {.type = token_comma, .valoare = 0, .column = lexer->column, .line = lexer->line};
            advance(lexer);
            return token;
        }
        if(c == ';'){
            skip_comment(lexer);
            continue;
        }
        if(c == ':'){
            Token token = {.type = token_colon, .valoare = 0, .column = lexer->column, .line = lexer->line};
            advance(lexer);
            return token;
        }
        if(c == '['){
            Token token = {.type = token_lbracket, .valoare = 0, .column = lexer->column, .line = lexer->line};
            advance(lexer);
            return token;
        }
        if(c == ']'){
            Token token = {.type = token_rbracket, .valoare = 0, .column = lexer->column, .line = lexer->line};
            advance(lexer);
            return token;
        }
        Token token = {.type = token_invalid, .valoare = 0, .column = lexer->column, .line = lexer->line};
        advance(lexer);
        return token;
        /*
         * TO DO : MACROS si DIRECTIVE in limita bunului simt
         */
        /* if(c == '.') return scan_for_directive(lexer); TO DO */
    }
}


