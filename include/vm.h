/*
*
*
*
*/
#ifndef VM_H
#define VM_H
#include <stdint.h>
#define MEMORY_SIZE 65536


//enum de stari (inspirat din y86-64 --csapp)
typedef enum{
    VM_AOK = 0,  
    VM_HALT,
    VM_ADDR,
    VM_INS
}vm_stare;



typedef struct{
    uint8_t memory[MEMORY_SIZE];
    uint16_t registers[15]; //registerFile cum s ar zice 
    uint16_t program_counter;
    uint8_t zero_flag;
    uint8_t sign_flag;
    uint8_t overflow_flag;
    vm_stare stare;

}VM;
/*
 * Relicve de la prima idee care n avea sens 
void vm_start(VM* vm);
vm_stare vm_push(VM* vm, uint16_t valoare); 
vm_stare vm_pop(VM* vm,uint16_t* valoare); 
*/ 
#endif
