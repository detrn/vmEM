#ifndef STATEMENT_H
#define STATEMENT_H
#include "token.h"

typedef enum{
    op_halt = 0,  
    op_nop,
    op_rrmov,
    op_irmov,
    op_rmmov,
    op_mrmov,
    op_add,
    op_sub,
    op_and,
    op_xor,
    op_jmp,
    op_jle,
    op_jl,
    op_je,
    op_jne,
    op_jge,
    op_jg,
    op_cmovle,
    op_cmovl,
    op_cmove,
    op_cmovne,
    op_cmovge,
    op_cmovg,
    op_call,
    op_ret,
    op_push,
    op_pop
}Opcode;

typedef enum{
    operand_label=0,
    operand_none,
    operand_register,
    operand_immediate,
    operand_memory
}OperandType;

typedef struct{
    uint8_t baza; 
    uint16_t offset;
}MemoryOperand;

typedef struct{
    OperandType type;
    union{
	uint8_t reg;
	MemoryOperand memory; 
	String label; //pentru jmp 
	uint16_t immediate; 
    };
}Operand;

typedef struct{
    Opcode opcode;
    //stiu ca n are sens sa fie srcB si ca e de fapt dest
    //dar deja am scris cam mult cu ideea asta si ramane asa 
    Operand srcA, srcB;
}Instruction;

typedef enum{
    st_instr=0,
    st_label,
    st_directive,
    st_invalid,
    st_eof
}StatementType; 

/*
typedef struct{
}Directive;
*/ 
typedef struct{
    StatementType type;
    union{
	Instruction instr;
	String label;
    //	Directive directive;
    };
}Statement;
#endif 

