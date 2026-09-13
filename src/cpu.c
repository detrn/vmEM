#include "../include/cpu.h"
#include "../include/vm.h"
#include "../include/instructions.h"
#include <sys/types.h>

typedef uint8_t byte; 

static vm_stare cpu_fetch(VM *vm, instruction_t* instruction);
static void cpu_decode(VM*vm, instruction_t* instruction,uint16_t* valA, uint16_t* valB);
static void cpu_execute(VM*vm, instruction_t* instruction);
static void cpu_memory(VM*vm, instruction_t* instruction);
static void pc_update(VM*vm,instruction_t* instruction);
static uint8_t valid_ifun(icode_t icode, uint8_t ifun);

vm_stare cpu_step(VM* vm){
    instruction_t instruction;
    uint16_t valA=0,valB=0;
    //cpu_fetch(): doc
    if(cpu_fetch(vm,&instruction)!=VM_AOK){
	return vm->stare;
    };
    //cpu_decode(): doc
    
    cpu_decode(vm,&instruction,&valA,&valB);

    //cpu_execute()
    //cpu_memory()
    //cpu_writeback()
    //cpu_pc_update()
    vm->stare = VM_AOK;
    return vm->stare;
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
    if(!valid_ifun(instruction->icode, instruction->ifun)) {
	vm->stare = VM_INS;
	return VM_INS;  
    }
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
	case RRMOVW:
	case OPW:
	case CMOVXX:
	case POPW:
	case PUSHW:
	    instruction_size = 2;
	    break;
	case IRMOVW:
	case RMMOVW:
	case MRMOVW:
	    instruction_size = 4;
	    break;
	case JXX:
	case CALL:
	    instruction_size = 3;
	    break;
	default: 
	    vm->stare = VM_INS;
	    return VM_INS;

    }
    if((uint32_t) instruction_size + vm->program_counter > 0xffff){
	vm->stare = VM_ADDR;
	return VM_ADDR;
    }
    switch(instruction->icode){
	case HALT:
	case NOP:
	case RET:
	    break;
	case CMOVXX:
	case RRMOVW:{
	    uint8_t secondByte = vm->memory[vm->program_counter + 1];
	    instruction->rA = secondByte >> 4; 
	    instruction->rB = secondByte & 0x0f;
	    if(instruction->rA ==NO_REG || instruction -> rB ==NO_REG){
		vm->stare = VM_INS;
		return VM_INS;
	    }
	    break;
	}
	case IRMOVW:{
	    uint8_t secondByte = vm->memory[vm->program_counter + 1];
	    instruction->rA = secondByte >>4; 
	    instruction->rB = secondByte & 0x0f;
	    if(instruction->rA != NO_REG || instruction->rB == NO_REG){
		vm->stare = VM_INS;
		return VM_INS;
	    }
	    uint8_t low_value = vm->memory[vm->program_counter + 2];
	    uint8_t high_value = vm->memory[vm->program_counter + 3];
	    instruction->valC = (uint16_t) low_value | ((uint16_t) high_value << 8);
	    break;
	}
	case RMMOVW:
	case MRMOVW:{
	    uint8_t secondByte = vm->memory[vm->program_counter + 1];
	    instruction->rA = secondByte >> 4; 
	    instruction->rB = secondByte & 0x0f; 
	    if(instruction->rA == NO_REG || instruction->rB == NO_REG){
		vm->stare = VM_INS;
		return VM_INS;
	    }
	    uint8_t low_value = vm->memory[vm->program_counter + 2];
	    uint8_t high_value = vm->memory[vm->program_counter + 3];
	    instruction->valC = (uint16_t) low_value | ((uint16_t) high_value << 8);
	    break;
	}
	case OPW:{
	    uint8_t secondByte = vm->memory[vm->program_counter + 1]; 
	    instruction->rA = secondByte >> 4; 
	    instruction->rB = secondByte & 0x0f;
	    if(instruction->rA == NO_REG || instruction->rB ==NO_REG){
		vm->stare = VM_INS;
		return VM_INS;
	    }
	    break;
	}
	case CALL:
	case JXX:{
	   uint8_t low_value = vm->memory[vm->program_counter + 1];
	   uint8_t high_value = vm->memory[vm->program_counter + 2]; 
	   instruction->valC = (uint16_t) low_value | ((uint16_t) high_value << 8);
	   break; 
	}
	case PUSHW:
	case POPW:{
	    uint8_t secondByte=vm->memory[vm->program_counter+1];
	    instruction->rA  = secondByte >> 4; 
	    instruction->rB = secondByte & 0x0f;
	    if(instruction->rA == NO_REG || instruction->rB !=NO_REG){
		vm->stare = VM_INS;
		return VM_INS;
	    }
	    break;

	}
	
    }
    instruction->valP = vm->program_counter + instruction_size;
    vm->stare = VM_AOK;
    return VM_AOK;
} 

static void cpu_decode(VM* vm, instruction_t* instruction,uint16_t* valA, uint16_t* valB){
    register_t srcA, srcB;
    switch(instruction->icode){
	case RRMOVW:
	case RMMOVW:
	case MRMOVW:
	case OPW:
	case CMOVXX:
	case PUSHW: 
	    srcA = instruction->rA; 
	    break;
	case CALL: 
	case RET:
	case POPW:
	    srcA = RSP;
	default: 
	    srcA = NO_REG;
    }
    switch(instruction->icode){
    
    }
}
   
