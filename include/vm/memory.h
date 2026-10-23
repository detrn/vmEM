#ifndef MEMORY_H 
#define MEMORY_H 
#include <stdint.h>
#include <stddef.h>
#define MEMORY_SIZE 65536
typedef enum{
    m_ok = 0, 
    m_invalid,

}memory_status;

/*
 * Memory holds the data the the  rest of the simulation uses 
 * Memory status represents the current status of the memory 
 */

typedef struct{
    uint8_t* data;
    memory_status status; 
}Memory;
/*
 * Memory access functions that are used to read and write
 * For read_word and read_byte the result of the reading is sent to the word and byte argument of the function 
 */
memory_status read_word(Memory* memory, uint16_t address, uint16_t* word);
memory_status write_word(Memory* memory,uint16_t address, uint16_t valA);
memory_status read_byte(Memory* memory, uint16_t address, uint8_t* byte);
memory_status write_byte(Memory* memory,uint16_t address, uint8_t valA);

/*
 * Loads source_data into the memory 
 */

memory_status loadMemory(Memory* memory, uint8_t* source_data, size_t size);

#endif 
