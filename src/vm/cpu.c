#include "vm/cpu.h"
#include "vm/vm.h"
#include "vm/instructions.h"
#include <stdint.h>
#include <sys/types.h>


static void alu(VM* vm,instruction_t* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE);
static vm_stare cpu_fetch(VM *vm, instruction_t* instruction);
static void cpu_decode(VM*vm, instruction_t* instruction,uint16_t* valA, uint16_t* valB);
static void cpu_execute(VM*vm, instruction_t* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE);
static vm_stare cpu_memory(VM* vm, instruction_t* instruction,uint16_t valA, uint16_t  *valE,uint16_t* valM);
static void cpu_writeback(VM* vm, instruction_t* instruction, uint16_t valE, uint16_t valM);
static void cpu_pc_update(VM*vm,instruction_t* instruction,uint16_t valM);
static uint8_t valid_ifun(icode_t icode, uint8_t ifun);
static void alu(VM* vm,instruction_t* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE);


uint8_t validareJMP(VM* vm, instruction_t* instruction);
uint8_t validareMove(VM* vm, instruction_t* instruction);

uint8_t checkOverflow(uint16_t valA, uint16_t valB,uint8_t ifun);
vm_stare cpu_step(VM* vm){
    instruction_t instruction;
    uint16_t valA=0,valB=0,valE=0,valM=0;
    //cpu_fetch(): doc
    if(cpu_fetch(vm,&instruction)!=VM_AOK){
	return vm->stare;
    };
    //cpu_decode(): doc
    
    cpu_decode(vm,&instruction,&valA,&valB);

    //cpu_execute()
    cpu_execute(vm,&instruction,&valA,&valB,&valE);
    //cpu_memory()
    if(cpu_memory(vm,&instruction,valA,&valE,&valM) !=VM_AOK) return vm->stare;
    //cpu_writeback()
    cpu_writeback(vm,&instruction,valE, valM);
    //cpu_pc_update()
    cpu_pc_update(vm,&instruction,valM);
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
	    vm->stare = VM_HALT;
	    return vm->stare;
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
    //determinare srcA 
    switch(instruction->icode){
	case RRMOVW:
	case RMMOVW:
	case OPW:
	case CMOVXX:
	case PUSHW: 
	    srcA = instruction->rA;
	    break;
	case MRMOVW:
	    srcA = instruction->rB;
	    break;
	case RET:
	case POPW: 
	    srcA = RSP;
	    break;
	default: 
	    srcA = NO_REG;
    }
    //determinare srcB
    switch(instruction->icode){
	case RMMOVW:
	case OPW:
	    srcB = instruction -> rB;
	    break;
	case POPW:
	    srcB = RSP;
	    break;
	case MRMOVW:
	    srcB = instruction ->rA;
	    break;
	case CALL:
	case PUSHW: 
	case RET:
	    srcB = RSP; 
	    break;
     	default: 
	    srcB =  NO_REG;
    }
    *valA = (srcA != NO_REG)?vm->registers[srcA]:0;
    *valB = (srcB != NO_REG)?vm->registers[srcB]:0;
}
//OF in y86 e doar pentru operatiile signed, nu si pentru cele unsigned (cam ciudat
uint8_t checkOverflow(uint16_t valA, uint16_t valB,uint8_t ifun){
    int16_t rez, a =(int16_t) valA, b=(int16_t)valB;
    switch(ifun){
	case 0:{//add 
	    rez = a + b; 
	    if((a > 0 && b > 0 && rez < 0) || (a < 0 && b < 0 && rez >0)) return 1;  
	    break;
	}
	case 1://sub
	    rez = b - a;
	    if((b > 0 && a < 0 && rez < 0) || (b < 0 && a > 0 && rez > 0)) return 1; 
	    break;
	default: 
	    return 0; 

    }
    return 0;
}
//Doar pentru OPW, deoarece pentru restul nu prea are sens (C ofera operatii matematice)
static void alu(VM* vm,instruction_t* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE){
    if(instruction->icode!=6) return;
    switch(instruction->ifun){
	case 0:{//add 
	    *valE = *valA + *valB;
	    break;
	}
	case 1://sub 
	    *valE = *valB - *valA; 
	    break;
	case 2://and
	     *valE = *valA & *valB; 
	    break;

	case 3: //xor 
	    *valE = *valA ^ *valB;
	    break;
	default:
	    *valE = 0; 
	    break;
    }
    //Muta le in cpu_execute ca n au ce cauta aici (desi practic asta face alu)
    //De fapt aici trebuiesc, deoarece doar opw modifica flagurile (restul instructiunilor se folosec de ele pentru decizii)
    vm->zero_flag = (*valE ==0); 
    vm->sign_flag = ((*valE & 0x8000)!=0); // 1000 0000
    vm->overflow_flag =checkOverflow(*valA, *valB, instruction->ifun);

}
static void cpu_execute(VM*vm, instruction_t* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE){
   switch(instruction->icode){
	case RET:
	    *valE = *valB + 2;
	    break;
	case RRMOVW:
	    *valE = 0 + *valA;
	    break;
	case OPW:
	    alu(vm,instruction,valA, valB,valE);
	    break;
	case CMOVXX:
	    *valE = 0 + *valA;
	    break;
	case POPW:
	    *valE = *valA + 2;
	    break;
	case PUSHW:
	    *valE = *valB - 2;
	    break;
	case IRMOVW:
	    *valE = 0 + instruction->valC;
	    break;
	case RMMOVW:
	    *valE = *valB + instruction->valC;
	    break;
	case MRMOVW:
	    *valE = *valA + instruction->valC;
	    break;
	case JXX:
	    *valE = 0 + instruction->valC;
	    break;
	case CALL:
	    *valE = *valB - 2;
	    break;
	default:
	    break;
   }
}

static vm_stare cpu_memory(VM* vm, instruction_t* instruction,uint16_t valA, uint16_t  *valE,uint16_t* valM){
    switch(instruction->icode){
	case PUSHW:
	    return write_word(vm, *valE, valA);
	case POPW:
	    return read_word(vm, valA, valM);
	case CALL:
	    return write_word(vm, *valE, instruction->valP);
	case RET:
	    return read_word(vm, valA, valM);
	case RMMOVW:
	    return write_word(vm, *valE, valA);
	case MRMOVW:
	    return read_word(vm, *valE, valM);
	default:
	    *valM = 0;
    }
    vm->stare = VM_AOK;
    return vm->stare;
}
uint8_t validareMove(VM* vm, instruction_t* instruction){
    if(instruction->icode!=0xc) return 0;
    switch(instruction->ifun){
	case 0: //movle 
	    return vm->zero_flag || (vm->sign_flag != vm->overflow_flag);
	    break;
	case 1://movl 
	    return vm->sign_flag!=vm->overflow_flag;
	    break;
	case 2://move 
	    return vm->zero_flag; 
	    break;
	case 3://movne
	    return !vm->zero_flag;
	    break;
	case 4://movge
	    return vm->sign_flag == vm->overflow_flag;
	    break;
	case 5://movg 
	    return !vm->zero_flag && (vm->sign_flag == vm->overflow_flag);
	    break;
	default:
	    return 0;
    }
}
uint8_t validareJMP(VM* vm, instruction_t* instruction){
    if(instruction->icode!=7) return 0; 
    switch(instruction->ifun){
	case 0:
	    return 1; 
	    break;
	case 1://jle
	    return (vm->sign_flag !=vm->overflow_flag) || vm->zero_flag;
	    break;
	case 2://jl 
	    return vm->sign_flag!=vm->overflow_flag; 
	    break;
	case 3://je 
	    return vm->zero_flag;
	    break;
	case 4://jne
	    return !vm->zero_flag;
	    break;
	case 5://jge
	    return vm->sign_flag == vm->overflow_flag;
	    break;
	case 6://jg 
	    return !vm->zero_flag && !(vm->sign_flag ^ vm->overflow_flag);
	    break;
	default:
	    return 0;
    }
}
static void cpu_writeback(VM* vm, instruction_t* instruction, uint16_t valE, uint16_t valM){
    switch(instruction->icode){
	case OPW:
	case RRMOVW:
	case IRMOVW:
	    vm->registers[instruction->rB] = valE;
	    break;
	case MRMOVW:
	    vm->registers[instruction->rA]=valM;
	    break;
	case CALL:
	case RET:
	case PUSHW:
	    vm->registers[RSP]=valE;
	    break;
	case POPW:
	    vm->registers[RSP]=valE;
	    vm->registers[instruction->rA]=valM;
	    break;
	case CMOVXX:
	    if(validareMove(vm,instruction)) vm->registers[instruction->rB]=valE;
	    break;
	default:
	    break;
    }
}

static void cpu_pc_update(VM* vm, instruction_t* instruction, uint16_t valM){
   switch(instruction->icode){
	case OPW:
	case RRMOVW:
	case IRMOVW:
	case MRMOVW:
	case PUSHW:
	case POPW:
	case CMOVXX:
	case RMMOVW:
	    vm->program_counter = instruction->valP;
	    break;
	case CALL:
	    vm->program_counter = instruction ->valC;
	    break;
	case RET:
	    vm->program_counter = valM;
	    break;
	case JXX:
	    if(validareJMP(vm,instruction)) vm->program_counter = instruction -> valC; 
	    else vm->program_counter = instruction -> valP;
	    break;
	default: 
	    vm->program_counter = instruction->valP;
   }
}
