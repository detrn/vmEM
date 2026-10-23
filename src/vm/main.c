#include "assembler/assembler.h"
#include "vm/vm.h"
#include "vm/memory.h"
#include "vm/debug.h"
#include <stdio.h>

int main(int argc, char** argv){
    if(argc == 0) return 1;
    Assembler assembler; 
    FILE* file = fopen(argv[1],"r");
    if(!assemblerInit(&assembler, file)){
	printf("Assembler failed to initialize\n");
	destroyAssembler(&assembler);
	return 1;
    }
    VM vm;
    if(!vm_init(&vm)){
	printf("Vm initialization error\n");
	goto end;
    }
    if(!encode(&assembler)){
	printf("Encode error\n");
	goto end;
    }
    memory_status m_status = loadMemory(&vm.memory,assembler.buffer.data ,assembler.buffer.size);
    if(m_status == m_invalid){
	printf("Invalid memory error\n");
	goto end;
    }
    run_vm(&vm);
    debug_dump_all(&vm);
end: 
    destroyVM(&vm);
    destroyAssembler(&assembler);
    return 0;
}
