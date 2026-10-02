#include "../include/vm.h"
#include <stdint.h>



vm_stare read_word(VM*vm, uint16_t address, uint16_t* word){
    if((uint32_t)address +1 >=MEMORY_SIZE){
	vm->stare = VM_ADDR;
	return vm->stare;
    }// verificam doi octeti liberi, pentru ca scriem un word, asadar avem nevoie sa fim siguri ca acel spatiu e liber
    uint8_t low = vm->memory[address];
    uint8_t high = vm->memory[address + 1];
    *word = low | (uint16_t)high << 8; 

    vm->stare = VM_AOK;
    return vm->stare; 
}
vm_stare write_word(VM* vm,uint16_t address, uint16_t valA){
    if((uint32_t)address +1 >=MEMORY_SIZE){
	vm->stare = VM_ADDR;
	return vm->stare;
    }
    uint8_t low = valA;
    uint8_t high= valA >> 8;
    vm->memory[address] = low;
    vm->memory[address + 1] = high;
    vm->stare = VM_AOK;
    return vm->stare; 
}
vm_stare read_byte(VM *vm, uint16_t address, uint8_t *byte){
    if((uint32_t) address + 1 >= MEMORY_SIZE){
	vm->stare = VM_ADDR;
	return vm->stare; 
    }
    *byte = vm->memory[address];
    vm->stare = VM_AOK;
    return vm->stare;
}
vm_stare write_byte(VM *vm, uint16_t address, uint8_t valA){
    if((uint32_t) address + 1 >= MEMORY_SIZE){
	vm->stare = VM_ADDR;
	return vm->stare; 
    }
    vm->memory[address] = valA;
    vm->stare = VM_AOK;
    return vm->stare;
}
