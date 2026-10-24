#ifndef DEBUG_H
#define DEBUG_H
#include "vm.h"

/*
 * Prints the amount of steps the cpu has executed, pc, cpu's status, flags and changes of the registers or the memory 
 */
void debug_dump_all(VM* vm);

#endif 
