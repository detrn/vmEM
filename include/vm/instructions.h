#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include <stdint.h>
#include "isa/isa.h"


/*
 * Possible function cases for opw 
 */

typedef enum{
    ADD=0,
    SUBW,
    ANDW,
    XORW
}ifun_op_t;

typedef enum{
    jmp=0,
    jle,
    jl,
    je,
    jne,
    jge,
    jg
}ifun_branches_t;

/*
 * Possible function cases for cmovxx 
 */

typedef enum {
    CMOVLE = 0,
    CMOVL,
    CMOVE,
    CMOVNE,
    CMOVGE,
    CMOVG
} ifun_moves_t;

/*
 * The instruction object represents the contract between the software ISA and 
 * the simulated hardware
 * icode is the first hexa digit and encodes the instruction name 
 * ifun is the second hexa digit and encodes the function of the instruction (if there is the case --otherwise it's 0)
 * rA:rB is the second byte of the instruction (each register is a hexa digit) 
 * valC occupies the last second bytes and encodes. It encodes an immediate, offset or address
 * valP it's the address of the program counter after incremeting it with the current encoding's size during the fetch 
 * process 
 */

typedef struct{
    icode_t icode;
    uint8_t ifun;
    reg_id_t rA;
    reg_id_t rB;
    uint16_t valC;
    uint16_t valP;
}Instruction;

#endif
