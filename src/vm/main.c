#include "../include/vm.h"
#include "../include/cpu.h"
#include "../include/debug.h"

int main(void)
{
    VM vm = {0};

    vm.memory[0] = 0x30;
    vm.memory[1] = 0xF0;
    vm.memory[2] = 0x34;
    vm.memory[3] = 0x12;
    vm.memory[4] = 0x00;

    vm.program_counter = 0;
    vm.stare = VM_AOK;

    cpu_step(&vm);
    cpu_step(&vm);

    debug_dump_all(&vm);

    return 0;
}
