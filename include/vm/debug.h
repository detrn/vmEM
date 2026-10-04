#ifndef DEBUG_H
#define DEBUG_H

#include "vm.h"
#include "assembler/token.h"
void debug_dump_all(const VM* vm);
void debug_token(const Token* token);

#endif 
