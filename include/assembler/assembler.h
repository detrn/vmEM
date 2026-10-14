#ifndef ASSEMBLER_H 
#define ASSEMBLER_H
#include "token.h"
#include "parser.h"
#include "lexer.h"



typedef struct{
    uint8_t* data; 
    size_t size;
    size_t byteBufCap;
}ByteBuffer;

typedef struct{
    String symbolName; 
    size_t location;
}Symbol;

typedef struct{
    Symbol* uSymbols;
    size_t size;
    size_t usTableCap;
}UnresolvedSymbolTable;

typedef struct{
    Symbol* symbols;
    size_t size;
    size_t simTableCap; 
}SymbolTable;

typedef struct{
    ByteBuffer buffer;
    SymbolTable symbolTable;
    UnresolvedSymbolTable uSymbolTable;
    Parser* parser;
    Lexer* lexer;

}Assembler;

void assemblerInit(Assembler* assembler, Parser* parser, Lexer* lexer);

void destroyAssembler(Assembler* assembler);

int encode(Assembler* assembler);

#endif 
