/*
*
*
*
*/
#ifndef vm_h
#define vm_h
#include <stdint.h>
#define MEMORY_SIZE 65536

typedef enum{
    NONE = 0,
    STACK_OVERFLOW = 1u << 1,
    STACK_UNDERFLOW = 1u << 2
}faults_t;

typedef enum{
    HALT = 0 ,
    SHUTDOWN,
    RUNNING
}stare_t;

typedef struct{
    uint8_t memory[MEMORY_SIZE];
    uint16_t program_counter;
    uint16_t stack_pointer;
    uint8_t zero_flag;
    stare_t stare;
    faults_t faults;
}VM;

void vm_start(VM* vm);
uint8_t vm_push(VM* vm, uint16_t valoare); 
uint8_t vm_pop(VM* vm,uint16_t* valoare); 
#endif
