#ifndef VM_H
#define VM_H
#include <stdint.h>
#include "vm/cpu.h"
#include "vm/memory.h"
#define MEMORY_SIZE 65536

/*
 * Vm is used as the bridge between the memory and the cpu 
 */

typedef struct{
    Memory memory;
    Cpu* cpu;

}VM;

/*
 * vm_init allocates memory for memory.data and cpu on the heap 
 * using. It also initializez cpu 
 */

int vm_init(VM* vm);

/*
 * Call cpu_step while cpu's status is aok. It also prints
 * useful information about pc and the opcode 
 */

void run_vm(VM* vm);

/*
 * Frees memory.data and the cpu
 */

void destroyVM(VM* vm);

#endif
