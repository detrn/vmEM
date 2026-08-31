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
    vm->stack_pointer =MEMORY_SIZE - 1;
    vm->zero_flag = 0;
    vm->stare = VM_AOK;


    /*
     * Memeoria e reprezentanta de memory, deci adresa e i din memory[i] (adresa virtuala!)
    */
    printf("Valoare la adresa de inceput: 0x%04" PRIx16 "\nStack_pointer: 0x%04" PRIx16 "\n", vm->memory[0], vm->stack_pointer);
}
vm_stare vm_push(VM *vm, uint16_t valoare){
    /*
    * val = 0x4321 -> in mem little endian: 0x21 43 (reprezentat, nu valoare)
    *  
    * De luat in calcul schimbarea sau conditionarea printfului cu un macro de debug pentru a nu scrie normal 
    * PENTRU POP LA FEL!
    *
    * REMINDER: stiva acum are acces la toata memoria. Pe viitor va avea acces la un segment limitat in memorie si va trebui schimbata verificarea
    */
    /*Refactor: am renuntat la faults. Pastram doar vm_stare. Pop si push vor returna corespunzator starea
*/
    if(vm->stare!= VM_AOK){
	return vm->stare;
    }
    if(vm->stack_pointer < 2 ){
	vm->stare = VM_ADDR; //adresa invalida (dam push la un octet cand mai avem doar un nibble disponibil
	return vm->stare;
    }
    uint8_t low_val = valoare; 
    uint8_t high_val = valoare >> 8;
    /*
    *	Little endian 
    *	SP indica urmatorul octet liber de dupa ceea ce exista deja pe stiva
    *	ex: sp: 0xFFFF (gol) -> push() sp: 0xFFFD (octetul liber, deoarece push impinge doi octeti la adresele 0xFFFF, 0xFFFE => urmatorul octet liber este 0xFFFD 
    */
    vm->memory[vm->stack_pointer--]=high_val;
    vm->memory[vm->stack_pointer--]=low_val;
    return vm->stare;
}
/*
 * Functia returneaza statusul executiei popului. Valoare de pop este transferata prin *val. 
 *
 */ 
vm_stare vm_pop(VM *vm, uint16_t* val){
    /*
    * Aducem sp la ultima valoare propriu zisa (formatul little endian, deci prima valoare pe care o accesam e low order bytte si urmeaza high order 
    * Functiile nu verifica intentionat daca VM este NULL. API-ul presupune ca VM este un obiect existent!
    *
    */

    /* Verificam daca masina poate procesa instructiuni (aok). Daca nu, returnam vm_stare. Daca val e null consideram instructiunea invalida => vm->stare = VM_INS */
    if(vm->stare != VM_AOK){
	return vm->stare;
    }
    if(val == NULL){
	vm->stare = VM_INS;
	return vm->stare;
    }
    if(vm->stack_pointer > MEMORY_SIZE - 1 ){
	vm->stare = VM_ADDR;
	return vm->stare;
    }
    uint8_t low_val = vm->memory[++vm->stack_pointer];
    uint8_t high_val = vm->memory[++vm->stack_pointer];
    uint16_t valoare =((uint16_t)high_val << 8 ) | low_val;  
    *val = valoare; 
    return vm->stare; 

}
