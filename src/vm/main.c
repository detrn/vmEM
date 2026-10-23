#include "assembler/assembler.h"
#include "vm/vm.h"
#include "vm/cpu.h"
#include "vm/memory.h"
#include "vm/debug.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    Assembler assembler; 
    FILE* file = fopen(argv[1],"r");
    if(!assemblerInit(&assembler, file)){
	printf("Assembler failed to initialize\n");
	destroyAssembler(&assembler);
	return 0;
    }
    VM vm = {0};
    vm.memory.data = calloc(MEMORY_SIZE,sizeof(uint8_t));
    if(!vm.memory.data){
	printf("Memory malloc error\n");
	destroyAssembler(&assembler);
	return 0;
    }
    Cpu cpu ={0};
    vm.cpu = &cpu;
    encode(&assembler);
    memory_status m_status = loadMemory(&vm.memory,assembler.buffer.data ,assembler.buffer.size);
    if(m_status == m_invalid){
	printf("Invalid memory error\n");
	return 0;
    }
    run_vm(&vm);
    debug_dump_all(&vm);
    free(vm.memory.data);
    printf("1\n");
    destroyAssembler(&assembler);
    return 0;
}
