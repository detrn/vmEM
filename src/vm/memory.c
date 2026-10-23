#include "vm/memory.h"
#include "vm/vm.h"
#include <string.h>

/*
 * IO functions for memory
 */

memory_status read_word(Memory* memory, uint16_t address, uint16_t* word){
    if((uint32_t)address +1 >=MEMORY_SIZE){
	memory->status= m_invalid;
	return memory->status;
    }
    uint8_t low = memory->data[address];
    uint8_t high = memory->data[address + 1];
    *word = low | (uint16_t)high << 8; 

    memory->status = m_ok;
    return memory->status; 
}
memory_status write_word(Memory* memory,uint16_t address, uint16_t valA){
    if((uint32_t)address +1 >=MEMORY_SIZE){
	memory->status= m_invalid;
	return memory->status;
    }
    uint8_t low = valA;
    uint8_t high= valA >> 8;
    memory->data[address] = low;
    memory->data[address + 1] = high;
    memory->status= m_ok;
    return memory->status; 
}


memory_status read_byte(Memory* memory, uint16_t address, uint8_t* byte){
    if((uint32_t) address + 1 >= MEMORY_SIZE){
	memory->status = m_invalid;
	return memory->status; 
    }
    *byte = memory->data[address];
    memory->status = m_ok;
    return memory->status;

}
memory_status write_byte(Memory* memory,uint16_t address, uint8_t valA){
    if((uint32_t) address + 1 >= MEMORY_SIZE){
	memory->status = m_invalid;
	return memory->status; 
    }
    memory->data[address] = valA;
    memory->status = m_ok;
    return memory->status;

}

memory_status loadMemory(Memory* memory, uint8_t* source_data, size_t size){
    if(!source_data) return m_invalid; 
    if(size + 1 > MEMORY_SIZE) return m_invalid; 
    memcpy(memory->data,source_data,size);
    memory->data[size] = '\0';
    return m_ok;
}
