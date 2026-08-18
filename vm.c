/*
*
*
*
*/
#include "vm.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
void vm_start(VM* vm){
    if(vm == NULL ){ 
	printf("VM unavailable\n");
	return;
    }
    vm->program_counter = 0u;
    /*
     * TO DO: 
     * de schimbat in memset()
     */
    for(size_t i = 0 ; i < MEMORY_SIZE; i++){
	vm->memory[i] = 0x0;
    }
    /*
     * Ultimul octet liber 
     */
    vm->stare = RUNNING;
    vm->stack_pointer =MEMORY_SIZE - 1;
    vm->zero_flag = 0;
    vm->stare = 2;
    vm->faults = 0; 

    /*
     * Memeoria e reprezentanta de memory, deci adresa e valoarea lui memory[i]
    */
    printf("Adresa inceput: 0x%04" PRIx16 "\nStack_pointer: 0x%04" PRIx16 "\n", vm->memory[0], vm->stack_pointer);
}
uint8_t vm_push(VM *vm, uint16_t valoare){
    /*
    * val = 0x4321 -> in mem little endian: 0x2143
    *  
    *
    */
    if(vm->stack_pointer - 2 < 0 ){
	printf("Stack overflow! Stack value: 0x%40
    }
    uint8_t low_val = valoare; 
    uint8_t high_val = valoare >> 8;
    /*
    *	Little endian 
    *	SP indica urmatorul octet liber de dupa ceea ce exista deja pe stiva 
    */
    vm->memory[vm->stack_pointer--]=high_val;
    vm->memory[vm->stack_pointer--]=low_val;
    return 0;
}
/*
 * Functia returneaza statusul executiei popului. Valoare de pop este transferata prin *val. 
 *
 */ 
uint8_t vm_pop(VM *vm, uint16_t* val){
    /*
    * Aducem sp la ultima valoare propriu zisa (formatul little endian, deci prima valoare pe care o accesam e low order bytte si urmeaza high order 
    * Functiile nu verifica intentionat daca VM este NULL. API-ul presupune ca VM este un obiect existent!
    *
    */  
    uint16_t sp = vm->stack_pointer;
    if(sp - 2 > MEMORY_SIZE){
	printf("Stack underflow! SP value:0x%40" PRIx16 "before pop\n", sp);
	return 1u<<1;
    }
    uint8_t low_val = vm->memory[++sp];
    uint8_t high_val = vm->memory[++sp];
    uint16_t valoare =((uint16_t)high_val << 8 ) | low_val;  
    vm->stack_pointer+=2;
    *val = valoare; 
    uint8_t status = 0; 
    return status; 
}
