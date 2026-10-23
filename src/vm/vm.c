#include "vm/vm.h"
#include "vm/cpu.h"
#include <stdio.h>

void run_vm(VM* vm){
    while(vm->cpu->status == CPU_AOK){

	printf("PC = 0x%04x, opcode = 0x%02x\n", vm->cpu->program_counter, vm->memory.data[vm->cpu->program_counter]);
	cpu_status status = cpu_step(vm->cpu,&vm->memory);
	if(status!=CPU_AOK){
	    vm->cpu->status = status;
	    return;
	}
    }
    return; 
}
