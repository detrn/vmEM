#ifndef ASSEMBLER_H 
#define ASSEMBLER_H
#include "token.h"
#include "parser.h"
#include "lexer.h"


/*
 * ByteBuffer is the result of the assembly process 
 * It contains the encoded instructions 
 * It is owned by the assembler which also frees the memory
 */
typedef struct{
    uint8_t* data; 
    size_t size;
    size_t byteBufCap;
}ByteBuffer;

/*
 * Symbol represents a label and location is where in the 
 * ByteBuffer it was encountered 
 */

typedef struct{
    String symbolName; 
    size_t location;
}Symbol;

/*
 * A table with the unresolved labels (labels that don't yet
 * have a ressolved address)
 * usTableCap is the actual allocated capacity of the table 
 * uSymbols is a symbol array that is allocated on the heap and managed 
 * by the assembler 
 */

typedef struct{
    Symbol* uSymbols;
    size_t size;
    size_t usTableCap;
}UnresolvedSymbolTable;

/*
 * Same as the earlier table, only that there are only resoled labels 
 */

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
    char* source; 
}Assembler;

/*
 * Assembler owns the memory of lexer, parser, the tables, source and buffer.data 
 * Assmebler frees the heap allocated memory
 * Assembler owns the file and the caller doesn't have to call close on it
 * It returns 1 on succes and 0 on failure. In case of an error the caller must call 
 * destroyAssembler in order to free the memory 
 * The sym tables are malloced in their own static functions 
 * File is the source file from which the assembler will read it's contents
 */

int assemblerInit(Assembler* assembler, FILE* file);

/*
 * Frees the symbol tables, buffer.data, parser, lexer and the source 
 */

void destroyAssembler(Assembler* assembler);

/*
 * Encodes the statements into the y86 machine code 
 */

int encode(Assembler* assembler);

#endif 
