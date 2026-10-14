#include "assembler/assembler.h"
#include "assembler/statement.h"
#include "assembler/token.h"
#include <string.h>
#include <stdlib.h>
#ifndef MAX_MEM 
#define MAX_MEM 65536
#endif 


//final 
void assemblerInit(Assembler *assembler, Parser *parser, Lexer *lexer){
    *assembler = (Assembler){0};
    assembler->parser = parser; 
    assembler->lexer = lexer; 
    
}
//final 
static Symbol* inSimTable(Assembler* assembler,String lname){
    for(size_t i = 0; i < assembler->symbolTable.size;i++){
	String tableLable = assembler->symbolTable.symbols[i].symbolName;
	if(lname.size ==tableLable.size && !strncmp(tableLable.p, lname.p,lname.size)) return &assembler->symbolTable.symbols[i];    
    }
    return NULL;
}


//final 
static int checkReserve(size_t size, ByteBuffer* buffer){
    if(size > MAX_MEM - buffer->size) return 0; // adica nu mai ai memoroe 
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
	uint8_t low = value;
	uint8_t high = value >> 8; 
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

//in constructie 
// return = 0 => STOP 
static int encodeInstruction(Assembler* assembler, Instruction* instruction){
    Opcode opcode = instruction->opcode;
    ByteBuffer* buffer = &assembler->buffer;//TO DO 
      switch(opcode){//verifrica daca scrierea a fost facut cu succes!!!
        case op_halt:
	    writeByte(0x0, buffer);
            break;
        case op_nop:
	    writeByte(0x10,buffer);
            break;
        case op_rrmov:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0x20,buffer);
	    writeByte((rA << 4) | rB, buffer);
            break;
	}
        case op_irmov:{ //conform parser srcA.immediate tine valoarea ce trebuie 
		       //scrisa in memorie 
	    uint16_t valoare = instruction->srcA.immediate;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0x30,buffer);
	    writeByte((0xf << 4) | rB, buffer);
	    writeWord(valoare, buffer);
            break;
	}
        case op_rmmov:{
	    uint8_t rA = instruction->srcA.reg;
	    uint16_t offsetB = instruction->srcB.memory.offset;
	    uint8_t bazaB = instruction->srcB.memory.baza;
	    writeByte(0x40 , buffer); 
	    writeByte((rA << 4) | bazaB, buffer);
	    writeWord(offsetB, buffer);
            break;
	}
        case op_mrmov:{
	    uint16_t offsetA = instruction->srcA.memory.offset;
	    uint8_t bazaA = instruction->srcA.memory.baza;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0x50, buffer);
	    writeByte((bazaA << 4) | rB, buffer);
	    writeWord(offsetA,buffer);
            break;
	}
        case op_add:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0x60,buffer);
	    writeByte((rA << 4) | rB,buffer);
            break;
	}
        case op_sub:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0x61,buffer);
	    writeByte((rA << 4) | rB,buffer);
            break;
	}
        case op_and:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0x62,buffer);
	    writeByte((rA << 4) | rB,buffer);
            break;
	}
        case op_xor:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0x63,buffer);
	    writeByte((rA << 4) | rB,buffer);
            break;
	}
	case op_call:
	case op_jmp:{
	    writeByte((opcode == op_jmp? 0x70:0x80),buffer);
	    if(instruction->srcB.type == operand_label){//pt documentatie: 
	    //nu ne intereseaza symbtable acum (ci la final rezolvam tot. acum doar adaugam
	    //toate labelurile in usymtab si revenim la el la final)
	    //insertUnresolvedTable();
		Symbol symbol = {.symbolName = instruction->srcB.label, .location = buffer->size};
		if(!insertUnresolvedTable(&assembler->uSymbolTable,symbol)) return 0;	
		writeWord(0x0000, buffer);
	    }
	    
	    else writeWord(instruction->srcB.immediate,buffer);

            break;
	}
        case op_jle:{
	    writeByte(0x71,buffer);
	    if(instruction->srcB.type == operand_label){
		Symbol symbol = {.symbolName = instruction->srcB.label, .location = buffer->size};
		if(!insertUnresolvedTable(&assembler->uSymbolTable,symbol)) return 0;	
		writeWord(0x0000, buffer);
	    }
	    else writeWord(instruction->srcB.immediate,buffer);
            break;
	}
        case op_jl:{
	    writeByte(0x72,buffer);
	    if(instruction->srcB.type == operand_label){
		Symbol symbol = {.symbolName = instruction->srcB.label, .location = buffer->size};
		if(!insertUnresolvedTable(&assembler->uSymbolTable,symbol)) return 0;	
		writeWord(0x0000, buffer);
	    }
	    else writeWord(instruction->srcB.immediate,buffer);
            break;
	}
            break;
        case op_je:{
	    writeByte(0x73,buffer);
	    if(instruction->srcB.type == operand_label){
		Symbol symbol = {.symbolName = instruction->srcB.label, .location = buffer->size};
		if(!insertUnresolvedTable(&assembler->uSymbolTable,symbol)) return 0;	
		writeWord(0x0000, buffer);
	    }
	    else writeWord(instruction->srcB.immediate,buffer);
            break;
	}
            break;
        case op_jne:{
	    writeByte(0x74,buffer);
	    if(instruction->srcB.type == operand_label){
		Symbol symbol = {.symbolName = instruction->srcB.label, .location = buffer->size};
		if(!insertUnresolvedTable(&assembler->uSymbolTable,symbol)) return 0;	
		writeWord(0x0000, buffer);
	    }
	    else writeWord(instruction->srcB.immediate,buffer);
            break;
	}
            break;
        case op_jge:{
	    writeByte(0x75,buffer);
	    if(instruction->srcB.type == operand_label){
		Symbol symbol = {.symbolName = instruction->srcB.label, .location = buffer->size};
		if(!insertUnresolvedTable(&assembler->uSymbolTable,symbol)) return 0;	
		writeWord(0x0000, buffer);
	    }
	    else writeWord(instruction->srcB.immediate,buffer);
            break;
	}
            break;
        case op_jg:{
	    writeByte(0x76,buffer);
	    if(instruction->srcB.type == operand_label){
		Symbol symbol = {.symbolName = instruction->srcB.label, .location = buffer->size};
		if(!insertUnresolvedTable(&assembler->uSymbolTable,symbol)) return 0;	
		writeWord(0x0000, buffer);
	    }
	    else writeWord(instruction->srcB.immediate,buffer);
            break;
	}
            break;
        case op_cmovle:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0xc0,buffer);
	    writeByte((rA << 4) | rB, buffer);
            break;
	}
        case op_cmovl:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0xc1,buffer);
	    writeByte((rA << 4) | rB, buffer);
            break;
	}
            break;
        case op_cmove:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0xc2,buffer);
	    writeByte((rA << 4) | rB, buffer);
            break;
	}
            break;
        case op_cmovne:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0xc3,buffer);
	    writeByte((rA << 4) | rB, buffer);
            break;
	}
            break;
        case op_cmovge:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0xc4,buffer);
	    writeByte((rA << 4) | rB, buffer);
            break;
	}
            break;
        case op_cmovg:{
	    uint8_t rA = instruction->srcA.reg;
	    uint8_t rB = instruction->srcB.reg;
	    writeByte(0xc5,buffer);
	    writeByte((rA << 4) | rB, buffer);
            break;
	}
            break;
        case op_ret:
	    writeByte(0x90,buffer);
            break;
        case op_push:{
	    writeByte(0xa0,buffer);
	    uint8_t regA = instruction->srcA.reg;
	    writeByte((regA << 4) | 0xf, buffer);
            break;
	}
        case op_pop:{
	    writeByte(0xb0,buffer);
	    uint8_t regB = instruction->srcB.reg;
	    writeByte((regB<<4)| 0xf, buffer);
            break;
	}
        default:
	    return 0;
            break;
    }
    return 1;
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
static int patchUnresolved(ByteBuffer* buffer, uint16_t location, uint16_t value){
    if(location > buffer->size) return 0; 
    buffer->data[location] = value;
    buffer->data[location + 1 ] = value >> 8; 
    return 1; 
}

static int resolveSymbols(Assembler* assembler){
    //iteram prin symboltable si l cautam in unresolvedSim table: daca l gasim scriem la buffer.symbols[locatieUnresolvedSim] adresa simbolului 
    //asta pentru fiecare intrare din tabel ca poate fi apelat de mai multe ori si sa nu stim de el 
    for(size_t i = 0 ; i < assembler->uSymbolTable.size; i++){
	String label_name = assembler->uSymbolTable.uSymbols[i].symbolName;
	size_t unresolvedLocation = assembler->uSymbolTable.uSymbols[i].location; //ar trebui sa fie locul din buffer in care am doi octeti de zero ce trebuiesc 
										  //suprascrisi   
	Symbol* foundSymbol = inSimTable(assembler,label_name);
	//acum avem simbolul, deci numele si locatia acestuia din SYMBOL_TABLE
	if(!foundSymbol) return 0; 
	//definitie: *buffer, unde in buffer, ce sa scrie n buffer 
	if(!patchUnresolved(&assembler->buffer,unresolvedLocation,foundSymbol->location)) return 0;
    }
    return 1;
}

//static void resolveSimbols(Assembler* assembler, )
int encode(Assembler* assembler){
    for(;;){
    Statement statement = parse(assembler->parser);
    switch(statement.type){
	case st_instr:
	    //poate fi apel la label: jmp ionel
	    //poate sa fie sau nu intalnit pana atunci 
		//il cautam in symbolTable. Daca e, atunci toate bune 
		//altfel il punem in UnresolvedSymbolTable + locatia in care trebuie scrisa 
		//adresa reala 
		//putem verifica direct daca exista sau nu, deoarece n are cum sa fie declarat acum 
	    if(!encodeInstruction(assembler, &statement.instr)) return 0;
	    break;
	case st_label:{ //forma label: (adica acum incep)
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
}

void destroyAssembler(Assembler* assembler){
    free(assembler->buffer.data);
    free(assembler->uSymbolTable.uSymbols);
    free(assembler->symbolTable.symbols);
    *assembler = (Assembler){0};
}

