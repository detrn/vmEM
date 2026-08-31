#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include <stdint.h>
typedef enum {
    RAX=0x0, 
    RCX=0x1, 
    RDX=0x2, 
    RBX= 0x3, 
    RSP=0x4, 
    RBP=0x5, 
    RSI=0x6, 
    RDI=0x7,
    R8=0x8,
    R9=0x9, 
    R10=0xa, 
    R11=0xb,
    R12=0xc,
    R13=0xd,
    R14=0xe,
    NO_REG=0xf
} reg_id_t;

typedef enum{
    HALT=0x0,
    NOP=0x1,
    RRMOVW=0x2,
    IRMOVW=0x3,
    RMMOVW=0x4,
    MRMOVW=0x5,
    OPW=0x6,
    JXX=0x7,
    CALL=0x8,
    RET=0x9,
    PUSHW=0xa,
    POPW=0xb,
    CMOVXX=0xc
}icode_t;

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

typedef enum {
    RRMOV = 0,
    CMOVLE,
    CMOVL,
    CMOVE,
    CMOVNE,
    CMOVGE,
    CMOVG
} ifun_moves_t;

typedef struct{
    icode_t icode;
    uint8_t ifun;
    reg_id_t rA;
    reg_id_t rB;
    uint16_t valC;
    uint16_t valP;
}instruction_t;

#endif
