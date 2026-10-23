#include "vm/cpu.h"
#include "vm/memory.h"
#include "vm/instructions.h"
#include <stdint.h>
#include <sys/types.h>


static void alu(Cpu* cpu,Instruction* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE);
static cpu_status cpu_fetch(Cpu* cpu,Memory* memory, Instruction* instruction);
static void cpu_decode(Cpu* cpu, Instruction* instruction,uint16_t* valA, uint16_t* valB);
static void cpu_execute(Cpu* cpu, Instruction* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE);
static cpu_status cpu_memory(Memory* memory, Instruction* instruction,uint16_t valA, uint16_t  *valE,uint16_t* valM);
static void cpu_writeback(Cpu* cpu, Instruction* instruction, uint16_t valE, uint16_t valM);
static void cpu_pc_update(Cpu* cpu,Instruction* instruction,uint16_t valM);
static uint8_t valid_ifun(icode_t icode, uint8_t ifun);
static void alu(Cpu* cpu,Instruction* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE);
static uint8_t validareJMP(Cpu* cpu, Instruction* instruction);
static uint8_t validareMove(Cpu* cpu, Instruction* instruction);
static uint8_t checkOverflow(uint16_t valA, uint16_t valB,uint8_t ifun);

cpu_status cpu_step(Cpu* cpu,Memory* memory){
    Instruction instruction;
    uint16_t valA=0,valB=0,valE=0,valM=0;
    //cpu_fetch(): doc
    cpu_status status = CPU_AOK;
    status = cpu_fetch(cpu,memory,&instruction);
    if(status!=CPU_AOK){
	cpu->status = status;
	return cpu->status;
    }
    //cpu_decode(): doc
    cpu_decode(cpu,&instruction,&valA,&valB);
    //cpu_execute()
    cpu_execute(cpu,&instruction,&valA,&valB,&valE);
    //cpu_memory()
    status = cpu_memory(memory,&instruction,valA,&valE,&valM); 
    if(status!=CPU_AOK){
	cpu->status = status;
	return cpu->status;
    }
    //cpu_writeback()
    cpu_writeback(cpu,&instruction,valE, valM);
    //cpu_pc_update()
    cpu_pc_update(cpu,&instruction,valM);
    cpu->status = CPU_AOK;
    cpu->steps++; 
    return cpu->status;
}
/*
 * Checks if the function encoding is valid for the ISA 
 */
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
            return ifun <= 5;

        default:
            return 0;
    }
}

/*
 * Fetches the first and second byte in order to know what instruction is encoded. 
 * Returns the status of the cpu without updating it 
 */
static cpu_status cpu_fetch(Cpu* cpu,Memory* memory, Instruction* instruction){
    uint8_t firstByte;
    memory_status status = read_byte(memory,cpu->program_counter,&firstByte);
    if(status !=m_ok) return CPU_ADDR;
    instruction->icode = firstByte >> 4; 
    instruction->ifun = firstByte & 0x0f;
    if(!valid_ifun(instruction->icode, instruction->ifun)) {
	return CPU_INS;  
    }
    uint8_t instruction_size;
    instruction->rA=NO_REG;
    instruction->rB=NO_REG;
    instruction->valC=0;
    instruction->valP=0;
    switch(instruction->icode){
	case HALT:
	    return CPU_HALT;
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
	    return CPU_INS;
    }
    if((uint32_t) instruction_size + cpu->program_counter > 0xffff){
	return CPU_ADDR;
    }
    status= m_ok;
    switch (instruction->icode) {
	case HALT:
	case NOP:
	case RET:
	    break;
	case CMOVXX:
	case RRMOVW: {
	    uint8_t secondByte;
	     status = read_byte(memory, cpu->program_counter + 1, &secondByte);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    instruction->rA = secondByte >> 4;
	    instruction->rB = secondByte & 0x0f;

	    if (instruction->rA == NO_REG || instruction->rB == NO_REG) {
		return CPU_INS;
	    }
	    break;
	}
	case IRMOVW: {
	    uint8_t secondByte;
	     status = read_byte(memory, cpu->program_counter + 1, &secondByte);

	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    instruction->rA = secondByte >> 4;
	    instruction->rB = secondByte & 0x0f;
	    if (instruction->rA != NO_REG || instruction->rB == NO_REG) {
		return CPU_INS;
	    }
	    uint8_t low_value;
	    uint8_t high_value;

	    status = read_byte(memory, cpu->program_counter + 2, &low_value);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    status = read_byte(memory, cpu->program_counter + 3, &high_value);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    instruction->valC = (uint16_t)low_value | ((uint16_t)high_value << 8);
	    break;
	}
	case RMMOVW:
	case MRMOVW: {
	    uint8_t secondByte;
	     status = read_byte(memory, cpu->program_counter + 1, &secondByte);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    instruction->rA = secondByte >> 4;
	    instruction->rB = secondByte & 0x0f;

	    if (instruction->rA == NO_REG || instruction->rB == NO_REG) {
		return CPU_INS;
	    }
	    uint8_t low_value;
	    uint8_t high_value;
	    status = read_byte(memory, cpu->program_counter + 2, &low_value);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    status = read_byte(memory, cpu->program_counter + 3, &high_value);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    instruction->valC = (uint16_t)low_value | ((uint16_t)high_value << 8);
	    break;
	}
	case OPW: {
	    uint8_t secondByte;
	     status = read_byte(memory, cpu->program_counter + 1, &secondByte);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    instruction->rA = secondByte >> 4;
	    instruction->rB = secondByte & 0x0f;
	    if (instruction->rA == NO_REG || instruction->rB == NO_REG) {
		return CPU_INS;
	    }
	    break;
	}
	case CALL:
	case JXX: {
	    uint8_t low_value;
	    uint8_t high_value;
	     status = read_byte(memory, cpu->program_counter + 1, &low_value);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    status = read_byte(memory, cpu->program_counter + 2, &high_value);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    instruction->valC = (uint16_t)low_value | ((uint16_t)high_value << 8);
	    break;
	}
	case PUSHW:
	case POPW: {
	    uint8_t secondByte;
	     status = read_byte(memory, cpu->program_counter + 1, &secondByte);
	    if (status != m_ok) {
		return CPU_ADDR;
	    }
	    instruction->rA = secondByte >> 4;
	    instruction->rB = secondByte & 0x0f;

	    if (instruction->rA == NO_REG || instruction->rB != NO_REG) {
		return CPU_INS;
	    }
	    break;
	}
    }
    instruction->valP = cpu->program_counter + instruction_size;
    return CPU_AOK;
} 

/*
 * Retrieves the values from the registers. It's built that way in order to simulate 
 * the behaviour of a register file 
 *
 */

static void cpu_decode(Cpu* cpu, Instruction* instruction,uint16_t* valA, uint16_t* valB){
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
	    srcA = instruction->rA;
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
	    srcB = instruction ->rB;
	    break;
	case CALL:
	case PUSHW: 
	case RET:
	    srcB = RSP; 
	    break;
     	default: 
	    srcB =  NO_REG;
    }
    *valA = (srcA != NO_REG)?cpu->registers[srcA]:0;
    *valB = (srcB != NO_REG)?cpu->registers[srcB]:0;
}
//OF in y86 e doar pentru operatiile signed, nu si pentru cele unsigned (cam ciudat
static uint8_t checkOverflow(uint16_t valA, uint16_t valB,uint8_t ifun){
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
/*
 * Built the simulate a real alu even though it wouldn't be necessary in this 
 * implementantion. It also updates the cpu's flags and valE 
 */
static void alu(Cpu* cpu,Instruction* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE){
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
    cpu->zero_flag = (*valE ==0); 
    cpu->sign_flag = ((*valE & 0x8000)!=0); // 1000 0000
    cpu->overflow_flag =checkOverflow(*valA, *valB, instruction->ifun);
}

/*
 * Computes valE which is a value that would usually need the intervention of the alu. 
 * I have used the alu only for the opw functions 
 */


static void cpu_execute(Cpu* cpu, Instruction* instruction,uint16_t*valA,uint16_t*valB,uint16_t*valE){
   switch(instruction->icode){
	case RET:
	    *valE = *valB + 2;
	    break;
	case RRMOVW:
	    *valE = 0 + *valA;
	    break;
	case OPW:
	    alu(cpu,instruction,valA, valB,valE);
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

/*
 * Reads or writes into the memory 
 */

static cpu_status cpu_memory(Memory* memory, Instruction* instruction,uint16_t valA, uint16_t  *valE,uint16_t* valM){
    memory_status status = m_ok; 
    switch(instruction->icode){
	case PUSHW:
	    status = write_word(memory, *valE, valA);
    	    break;
	
	case POPW:
	    status =read_word(memory, valA, valM);
	    break;
	case CALL:
	    status = write_word(memory, *valE, instruction->valP);
	    break;
	case RET:
	    status = read_word(memory, valA, valM);
	    break;
	case RMMOVW:
	    status = write_word(memory, *valE, valA);
	    break;
	case MRMOVW:
	    status = read_word(memory, *valE, valM);
	    break;
	default:
	    *valM = 0;
	    break;
    }
    if(status!=m_ok){
	return CPU_ADDR;
    }
    return CPU_AOK;
}

uint8_t validareMove(Cpu* cpu, Instruction* instruction){
    if(instruction->icode!=0xc) return 0;
    switch(instruction->ifun){
	case 0: //movle 
	    return cpu->zero_flag || (cpu->sign_flag != cpu->overflow_flag);
	    break;
	case 1://movl 
	    return cpu->sign_flag!=cpu->overflow_flag;
	    break;
	case 2://move 
	    return cpu->zero_flag; 
	    break;
	case 3://movne
	    return !cpu->zero_flag;
	    break;
	case 4://movge
	    return cpu->sign_flag == cpu->overflow_flag;
	    break;
	case 5://movg 
	    return !cpu->zero_flag && (cpu->sign_flag == cpu->overflow_flag);
	    break;
	default:
	    return 0;
    }
}
uint8_t validareJMP(Cpu* cpu, Instruction* instruction){
    if(instruction->icode!=7) return 0; 
    switch(instruction->ifun){
	case 0:
	    return 1; 
	    break;
	case 1://jle
	    return (cpu->sign_flag !=cpu->overflow_flag) || cpu->zero_flag;
	    break;
	case 2://jl 
	    return cpu->sign_flag!=cpu->overflow_flag; 
	    break;
	case 3://je 
	    return cpu->zero_flag;
	    break;
	case 4://jne
	    return !cpu->zero_flag;
	    break;
	case 5://jge
	    return cpu->sign_flag == cpu->overflow_flag;
	    break;
	case 6://jg 
	    return !cpu->zero_flag && !(cpu->sign_flag ^ cpu->overflow_flag);
	    break;
	default:
	    return 0;
    }
}

static void cpu_writeback(Cpu* cpu, Instruction* instruction, uint16_t valE, uint16_t valM){
    switch(instruction->icode){
	case OPW:
	case RRMOVW:
	case IRMOVW:
	    cpu->registers[instruction->rB] = valE;
	    break;
	case MRMOVW:
	    cpu->registers[instruction->rB]=valM;
	    break;
	case CALL:
	case RET:
	case PUSHW:
	    cpu->registers[RSP]=valE;
	    break;
	case POPW:
	    cpu->registers[RSP]=valE;
	    cpu->registers[instruction->rA]=valM;
	    break;
	case CMOVXX:
	    if(validareMove(cpu,instruction)) cpu->registers[instruction->rB]=valE;
	    break;
	default:
	    break;
    }
}

static void cpu_pc_update(Cpu* cpu, Instruction* instruction, uint16_t valM){
   switch(instruction->icode){
	case OPW:
	case RRMOVW:
	case IRMOVW:
	case MRMOVW:
	case PUSHW:
	case POPW:
	case CMOVXX:
	case RMMOVW:
	    cpu->program_counter = instruction->valP;
	    break;
	case CALL:
	    cpu->program_counter = instruction ->valC;
	    break;
	case RET:
	    cpu->program_counter = valM;
	    break;
	case JXX:
	    if(validareJMP(cpu,instruction)) cpu->program_counter = instruction -> valC; 
	    else cpu->program_counter = instruction -> valP;
	    break;
	default: 
	    cpu->program_counter = instruction->valP;
   }
}
