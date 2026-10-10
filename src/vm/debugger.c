#include "vm/vm.h"
#include <stdint.h>
#include <stdio.h>


static const char *register_names[15] = {
    "rax", "rcx", "rdx", "rbx",
    "rsp", "rbp", "rsi", "rdi",
    "r8", "r9", "r10", "r11",
    "r12", "r13", "r14"
};
static const char* status_words[4]={
    "VM_AOK",
    "VM_HALT",
    "VM_ADDR",
    "VM_INS"
};
// void debug_print_registers(const VM* vm){}

    
void debug_dump_all(const VM* vm){
    static uint32_t steps = 0; 
    if(vm->stare != VM_AOK){
	printf(" -- Debug Print Screen --\n");
	printf("The VM worked for %d steps and it has stopped at PC address 0x%04x. Status: %s. Flags: ZF=%d, SF=%d, OF=%d\n",
		steps,vm->program_counter,status_words[vm->stare],vm->zero_flag,vm->sign_flag,vm->overflow_flag);
	printf("Registrii schimbati:\n");
	for(int i = 0; i < 15; i++){
	    if(vm->registers[i] != 0){
		printf("%%%s: 0x0000 -> 0x%04x\n",register_names[i],vm->registers[i]); //stiu ca sunt aceeasi valoare. cum afisez pentru enum? 
	    }
	}
	printf("Modificari memorie:\n");
	for(int i = 0 ; i < MEMORY_SIZE;i++){
	    if(vm->memory[i]!=0){
		
		printf("0x%04x: 0x0000 -> 0x%02x\n",i,vm->memory[i]);
	    }
	}
    } 
    steps++; 
    }

//void debug_print_flags(const VM* vm){}
