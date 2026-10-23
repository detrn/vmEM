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
    uint32_t steps; 
}VM;

void run_vm(VM* vm);

#endif
