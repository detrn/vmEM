#include "vm/vm.h"
#include "vm/cpu.h"
#include "vm/memory.h"
#include <stdio.h>
#include <stdlib.h>


int vm_init(VM* vm){
    *vm = (VM){0};
    vm->memory.data = calloc(MEMORY_SIZE,sizeof(uint8_t));
    if(!vm->memory.data){
	printf("Memory allocation error\n");
	return 0;
    } 
    Cpu* cpu = malloc(sizeof(Cpu));
    vm->cpu = cpu;
    if(!cpu){
	printf("Cpu allocation error\n");
	return 0; 
    }
    *cpu = (Cpu){0};
    return 1;
}

void run_vm(VM* vm){
    while(vm->cpu->status == CPU_AOK){
	printf("PC = 0x%04x, opcode = 0x%02x\n", vm->cpu->program_counter, vm->memory.data[vm->cpu->program_counter]);
	cpu_status status = cpu_step(vm->cpu,&vm->memory);
	if(status!=CPU_AOK){
	    vm->cpu->status = status;
	    return;
	}
    }
}

void destroyVM(VM* vm){
    free(vm->memory.data);
    free(vm->cpu);
}
