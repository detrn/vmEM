#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include "vm/memory.h"
/*
 * AOK = everything is ok 
 * HALT = stop 
 * ADDR = invalid address 
 * INS = invalid instruction 
 */

typedef enum{
    CPU_AOK = 0,
    CPU_HALT,
    CPU_ADDR,
    CPU_INS 
}cpu_status; 

/*
 * Cpu represents the simulated processing unit. It stores the 
 * value of the registers in the array registers (this is why I choose 
 * a 16bit representation)
 * The program counter and the flags 
 * Steps represents the amount of steps the cpu has done and is only 
 * used for diagnosis
 */

typedef struct{
    uint16_t registers[15]; 
    uint16_t program_counter; 
    uint8_t zero_flag; 
    uint8_t sign_flag; 
    uint8_t overflow_flag; 
    cpu_status status; 
    uint32_t steps; 
}Cpu; 

/*
 * The function executes a complete step, meaning the complete 
 * fetch -> decode -> execute -> memory -> writeback and update cycle 
 * It returns the state of the cpu for error signaling 
 * The second parameter is a pointer to the memory of the VM. 
 */

cpu_status cpu_step(Cpu* cpu, Memory* memory);

#endif 

