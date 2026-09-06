#ifndef CPU_C 
#define CPU_C 
#include "../include/cpu.h"
#include "../include/cpu.h"
#include "../include/vm.h"
#include "../include/instructions.h"
#include <__stdarg_va_list.h>

typedef uint8_t byte; 

static vm_stare cpu_fetch(VM *vm, instruction_t* instruction);
static void cpu_decode(VM*vm, instruction_t* instruction);
static void cpu_execute(VM*vm, instruction_t* instruction);
static void cpu_memory(VM*vm, instruction_t* instruction);
static void pc_update(VM*vm,instruction_t* instruction);
static uint8_t valid_ifun(icode_t icode, uint8_t ifun);

vm_stare cpu_step(VM* vm){
    instruction_t instruction; 
    return cpu_fetch(vm,&instruction);
}

static uint8_t valid_ifun(icode_t icode, uint8_t ifun){
    switch (icode) {
        case HALT:
        case NOP:
        case RRMOVW:
        case IRMOVW:
        case RMMOVW:
        case MRMOVW:
        case CALL:
        case RET:
        case PUSHW:
        case POPW:
            return ifun == 0;

        case OPW:
            return ifun <= 3;

        case JXX:
        case CMOVXX:
            return ifun <= 6;

        default:
            return 0;
    }
}
static vm_stare cpu_fetch(VM* vm, instruction_t* instruction){
    uint8_t firstByte = vm->memory[vm->program_counter];    
    instruction->icode = firstByte >> 4; 
    instruction->ifun = firstByte & 0x0f;
    if(!valid_ifun(instruction->icode, instruction->ifun)) return VM_INS;  
    uint8_t instruction_size;
    instruction->rA=NO_REG;
    instruction->rB=NO_REG;
    instruction->valC=0;
    instruction->valP=0;
    
    switch(instruction->icode){
	case HALT:
	case NOP:
	case RET:
	    instruction_size = 1; 
	    break;
	case CMOVXX:
	case RRMOVW:{
	    uint8_t secondByte = vm->memory[vm->program_counter + 1];
	    instruction->rA = secondByte >> 4; 
	    instruction->rB = secondByte & 0x0f;
	    instruction_size = 2;
	    break;
	}
	case IRMOVW:{
	    uint8_t secondByte = vm->memory[vm->program_counter + 1];
	    instruction->rA = secondByte >>4; 
	    if(instruction->rA != NO_REG) return VM_INS;
	    instruction->rB = secondByte & 0x0f;
	    if(instruction->rB == NO_REG) return VM_INS;
	    uint8_t low_value = vm->memory[vm->program_counter + 2];
	    uint8_t high_value = vm->memory[vm->program_counter + 3];
	    instruction->valC = (uint16_t) low_value | ((uint16_t) high_value << 8);
	    instruction_size = 4; //irmovw, word = 2 octeti. Nu avem opt octeti ca n y86_64. Poate pe viitor  
	    break;
	}
	case RMMOVW:
	case MRMOVW:{
	    uint8_t secondByte = vm->memory[vm->program_counter + 1];
	    instruction->rA = secondByte >> 4; 
	    if(instruction->rA == NO_REG) return VM_INS;
	    instruction->rB = secondByte & 0x0f; 
	    if(instruction->rB == NO_REG) return VM_INS;
	    uint8_t low_value = vm->memory[vm->program_counter + 2];
	    uint8_t high_value = vm->memory[vm->program_counter + 3];
	    instruction->valC = (uint16_t) low_value | ((uint16_t) high_value << 8);
	    instruction_size = 4;
	    break;
	}
	case OPW:{
	    uint8_t secondByte = vm->memory[vm->program_counter + 1]; 
	    instruction->rA = secondByte >> 4; 
	    if(instruction->rA == NO_REG) return VM_INS;
	    instruction->rB = secondByte & 0x0f;
	    if(instruction->rB == NO_REG) return VM_INS;
	    instruction_size = 2;
	    break;
	}
	case CALL:
	case JXX:{
	   uint8_t low_value = vm->memory[vm->program_counter + 1];
	   uint8_t high_value = vm->memory[vm->program_counter + 2]; 
	   instruction->valP = (uint16_t) low_value | ((uint16_t) high_value << 8);
	   instruction_size = 3;
	   break; 
	}
	
    }
}  
   
#endif 
