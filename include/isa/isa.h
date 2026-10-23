#ifndef ISA_H
#define ISA_H

/*
 * All possible registers the software ISA implements
 * NO_REG indicates the absence of a register
 */
typedef enum {
    RAX = 0x0,
    RCX = 0x1,
    RDX = 0x2,
    RBX = 0x3,
    RSP = 0x4,
    RBP = 0x5,
    RSI = 0x6,
    RDI = 0x7,
    R8 = 0x8,
    R9 = 0x9,
    R10 = 0xa,
    R11 = 0xb,
    R12 = 0xc,
    R13 = 0xd,
    R14 = 0xe,
    NO_REG = 0xf
} reg_id_t;

/*
 * All possible instructions the software ISA implements
 * JXX, OPW and CMOVXX have more cases
 */
typedef enum {
    HALT = 0x0,
    NOP = 0x1,
    RRMOVW = 0x2,
    IRMOVW = 0x3,
    RMMOVW = 0x4,
    MRMOVW = 0x5,
    OPW = 0x6,
    JXX = 0x7,
    CALL = 0x8,
    RET = 0x9,
    PUSHW = 0xa,
    POPW = 0xb,
    CMOVXX = 0xc
} icode_t;

#endif
