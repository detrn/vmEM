#include "assembler/assembler.h"
#include "assembler/parser.h"
#include "assembler/statement.h"
#include "assembler/token.h"
#include "isa/isa.h"
#include <endian.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#ifndef MAX_MEM 
#define MAX_MEM 65536
#endif 



int assemblerInit(Assembler *assembler, FILE* file){
    if(!assembler) return 0;
    *assembler = (Assembler){0};
    if(!file){
	return 0;
    }
    int aux = fseek(file,0,SEEK_END);
    if(aux!=0){
	fclose(file);
	return 0;
    }
    long file_size = ftell(file);
    if(file_size == -1){
	fclose(file);
	return 0;
    }
    if(fseek(file,0,SEEK_SET)!=0){ //seek_set = start of the file 
	fclose(file);
	return 0;
    }
    assembler->source = malloc( (size_t)file_size + 1);
    if(!assembler->source){
	fclose(file);
	return 0;
    }
    size_t temp = fread(assembler->source,1,(size_t) file_size,file);
    if(temp!=(size_t)file_size){
	fclose(file);
	return 0;
    }
    fclose(file);
    assembler->source[file_size] = '\0'; 
    assembler->lexer = malloc(sizeof(Lexer));
    if(!assembler->lexer) return 0; 
    lexer_init(assembler->lexer, assembler->source);
    assembler->parser = malloc(sizeof(Parser));
    if(!assembler->parser) return 0;
    parser_init(assembler->parser, assembler->lexer);
    return 1;    
}
//final 
static Symbol* inSimTable(Assembler* assembler,String lname){
    for(size_t i = 0; i < assembler->symbolTable.size;i++){
	String tableLable = assembler->symbolTable.symbols[i].symbolName;
	if(lname.size ==tableLable.size && !strncmp(tableLable.p, lname.p,lname.size)) return &assembler->symbolTable.symbols[i];    
    }
    return NULL;
}

static int checkReserve(size_t size, ByteBuffer* buffer){
    if(buffer->size > MAX_MEM || size > MAX_MEM - buffer->size) return 0; // adica nu mai ai memoroe 
    if(buffer->size + size <= buffer->byteBufCap) return 1; 
    size_t capacity = buffer->byteBufCap ? buffer->byteBufCap : 16; 
    size_t required_memory = buffer->size + size; 
    while(required_memory > capacity){
	if(capacity > MAX_MEM/2){
	    capacity = required_memory;
	    break;
	}
	capacity *=2; // 16 32 64 ... 
    }
    uint8_t* data = realloc(buffer->data, sizeof(*buffer->data) * capacity);
    if(!data) return 0; 
    buffer->data = data; 
    buffer->byteBufCap = capacity;
    return 1;
}
//final 
static int writeByte(uint8_t value, ByteBuffer* bBuffer){
    if(checkReserve(1,bBuffer)){ 
	bBuffer->data[bBuffer->size++] = value;
	return 1;
    }
    return 0;
}
//finial
static int writeWord(uint16_t value, ByteBuffer* bBuffer){
    if(checkReserve(2,bBuffer)){
	uint8_t low = (uint8_t)value;
	uint8_t high = (uint8_t)(value >> 8); 
    	bBuffer->data[bBuffer->size++] = low;
	bBuffer->data[bBuffer->size++] = high; //LITTLE ENDIAN ATENTIE: CA LA CPU.C 
	return 1;			       //VERIFICA
    }
    return 0; 
}
//final

static int insertUnresolvedTable(UnresolvedSymbolTable* table, Symbol symbol){
    if( table->size == table->usTableCap){
	size_t capacity = table->usTableCap? table->usTableCap * 2 : 16;
	Symbol* temp = realloc(table->uSymbols, sizeof(*table->uSymbols) * capacity);
	if(!temp) return 0;
	table->uSymbols = temp;
	table->usTableCap = capacity;
    }
    
    table->uSymbols[table->size++] = symbol;
    return 1;
}

/*
 * The function encodes the instruction. It firstly finds the icode 
 * and then writes it with ifun. The second switch encodes the rest of 
 * the instruction 
 * Return 1 in case of succes 
 */

static int encodeInstruction(Assembler* assembler, Instruction* instruction){
    Opcode op = instruction->opcode;
    uint8_t instructionByte; 
    ByteBuffer* buffer =&assembler->buffer;
    int stop = 1 ;
    switch(op){
	case op_halt:
   	    instructionByte = 0x00;
	    break;
	case op_nop:
    	    instructionByte = 0x10;
	    break;
	case op_rrmov:
  	    instructionByte = 0x20;
	    break;
	case op_irmov:
  	    instructionByte = 0x30;
	    break;
	case op_rmmov:
  	    instructionByte = 0x40;
	    break;
	case op_mrmov:
  	    instructionByte = 0x50;
	    break;
	case op_add:
    	    instructionByte = 0x60;
	    break;
	case op_sub:
    	    instructionByte = 0x61;
	    break;
	case op_and:
    	    instructionByte = 0x62;
	    break;
	case op_xor:
    	    instructionByte = 0x63;
	    break;
	case op_jmp:
    	    instructionByte = 0x70;
	    break;
	case op_jle:
    	    instructionByte = 0x71;
	    break;
	case op_jl:
     	    instructionByte = 0x72;
	    break;
	case op_je:
     	    instructionByte = 0x73;
	    break;
	case op_jne:
    	    instructionByte = 0x74;
	    break;
	case op_jge:
    	    instructionByte = 0x75;
	    break;
	case op_jg:
     	    instructionByte = 0x76;
	    break;
	case op_call:
   	    instructionByte = 0x80;
	    break;
	case op_ret:
    	    instructionByte = 0x90;
	    break;
	case op_push:
   	    instructionByte = 0xa0;
	    break;
	case op_pop:
    	    instructionByte = 0xb0;
	    break;
	case op_cmovle:
 	    instructionByte = 0xc0;
	    break;
	case op_cmovl:
  	    instructionByte = 0xc1;
	    break;
	case op_cmove:
  	    instructionByte = 0xc2;
	    break;
	case op_cmovne:
 	    instructionByte = 0xc3;
	    break;
	case op_cmovge:
 	    instructionByte = 0xc4;
	    break;
	case op_cmovg:
  	    instructionByte = 0xc5;
	    break;
	default: 
	    return 0;
    }
    uint8_t icode = instructionByte >> 4;
    stop = writeByte(instructionByte,buffer);
    if(!stop) return 0;
    switch((icode_t)icode){
	case HALT:
	case NOP:
	case RET:
            break;
	case RRMOVW:
	case OPW: 
	case CMOVXX:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    stop =writeByte((uint8_t)((rA << 4) | rB), buffer);
	    if(!stop) return 0;
	    break;
	}
	case IRMOVW:{ //conform parser srcA.immediate tine valoarea ce trebuie 
		       //scrisa in memorie 
	    uint16_t valoare = instruction->srcA.immediate;
	    uint8_t rB = instruction->srcB.reg;
	    stop = writeByte((0xf << 4) | rB, buffer);
	    if(!stop) return 0;
	    stop = writeWord(valoare, buffer);
	    if(!stop) return 0; 
            break;
	}
	case RMMOVW:{
	    uint8_t rA = instruction->srcA.reg;
	    uint16_t offsetB = instruction->srcB.memory.offset;
	    uint8_t bazaB = instruction->srcB.memory.baza;
	    stop = writeByte((uint8_t)((rA << 4) | bazaB), buffer);
	    if(!stop) return 0;
	    stop = writeWord(offsetB, buffer);
	    if(!stop) return 0;
            break;
	}
	case MRMOVW:{ 
			//rA    rB 
	    // mrmovw d[%rax], %rsi 
	    uint16_t offsetA = instruction->srcA.memory.offset;
	    uint8_t bazaA = instruction->srcA.memory.baza;
	    uint8_t rB = instruction->srcB.reg;
	    stop = writeByte((uint8_t)((bazaA << 4) | rB), buffer);
	    if(!stop) return 0;
	    stop = writeWord(offsetA,buffer);
	    if(!stop) return 0;
            break;
	}
	/*
	 * We check if the symbol (currently only labels) are called or defined
	 * If there are only called but not defined we add them to 
	 * UnresolvedSymbolTable 
	 * Otherwise they are added to the symbol table 
	 */
	case JXX:
	case CALL:{
	    if(instruction->srcB.type == operand_label){
		Symbol symbol = {.symbolName = instruction->srcB.label, .location = buffer->size};
		if(!insertUnresolvedTable(&assembler->uSymbolTable,symbol)) return 0;	
		stop = writeWord(0x0000, buffer);
		if(!stop) return 0;
	    }
	    else{
		stop = writeWord(instruction->srcB.immediate,buffer);
		if(!stop) return 0;
	    }
            break;
	}
	case PUSHW:{
	    uint8_t regA = instruction->srcA.reg;
	    stop = writeByte((uint8_t)((regA << 4) | 0xf), buffer);
	    if(!stop) return 0;
            break;
	}
        case POPW:{
	    uint8_t regB = instruction->srcB.reg;
	    stop = writeByte((uint8_t)((regB<<4)| 0xf), buffer);
	    if(!stop) return 0;
            break;
	}
     	default:
	    stop = 0;
	    break;
    }
    return stop; 
}

static int insertSymbolTable(SymbolTable* table, Symbol symbol){
    if(table->size == table->simTableCap){
	size_t capacity = table->simTableCap? table->simTableCap* 2: 16;
	Symbol* temp = realloc(table->symbols,sizeof(*table->symbols)* capacity);
	if(!temp) return 0;
	table->symbols = temp; 
	table->simTableCap = capacity;
    }
    table->symbols[table->size++] = symbol;
    return 1;
}

/*
 * Inserts the new found address of the symbol (the resolved one) in the buffer
 * at the specified address 
 */

static int patchUnresolved(ByteBuffer* buffer, size_t location, size_t value){
    if(location > buffer->size) return 0; 
    buffer->data[location] =(uint8_t)value;
    buffer->data[location + 1 ] =(uint8_t)( value >> 8); 
    return 1; 
}

/*
 * The function firstly iterates through the unresolved table and searches 
 * it in the resolved one. If the unresolved symbol has been found it is then
 * patched 
 */

static int resolveSymbols(Assembler* assembler){
        for(size_t i = 0 ; i < assembler->uSymbolTable.size; i++){
	String label_name = assembler->uSymbolTable.uSymbols[i].symbolName;
	size_t unresolvedLocation = assembler->uSymbolTable.uSymbols[i].location;
	Symbol* foundSymbol = inSimTable(assembler,label_name);
	//acum avem simbolul, deci numele si locatia acestuia din SYMBOL_TABLE
	if(!foundSymbol) return 0; 
	//definitie: *buffer, unde in buffer, ce sa scrie n buffer 
	if(!patchUnresolved(&assembler->buffer,unresolvedLocation,foundSymbol->location)) return 0;
    }
    return 1;
}

int encode(Assembler* assembler){
    for(;;){
	Statement statement = parse(assembler->parser);
	switch(statement.type){
	    case st_instr:
		if(!encodeInstruction(assembler, &statement.instr)) return 0;
		break;
	    case st_label:{ //forma label: (adica acum incep)
		    if(assembler->buffer.size >= MAX_MEM) return 0;
		    Symbol symbol = { .symbolName = statement.label, .location = assembler->buffer.size};
		    if(inSimTable(assembler,statement.label)){
			//duplicate label
			return 0;
		    }
		    if(!insertSymbolTable(&assembler->symbolTable,symbol)) return 0;
		    break;
	    }
	    case st_eof:
	       return resolveSymbols(assembler);
	    default:
		return 0;
		break;
	}
    }
    return 1;
}

void destroyAssembler(Assembler* assembler){
    free(assembler->buffer.data);
    free(assembler->uSymbolTable.uSymbols);
    free(assembler->symbolTable.symbols);
    free(assembler->parser);
    free(assembler->lexer);
    free(assembler->source);
    *assembler = (Assembler){0};
}

