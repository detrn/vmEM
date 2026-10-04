#include "assembler/token.h"
#include "vm/vm.h"
#include "vm/cpu.h"
#include "vm/debug.h"
#include "assembler/lexer.h"
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    VM vm = {0};
    FILE* file = fopen("test.myasm","r");
    if(!file){
	printf("Nu s a putut deschide fisierul\n");
	return -1;
    }
    fseek(file,0,SEEK_END);
    long size = ftell(file);
    rewind(file);
    char *source = malloc((size_t)size + 1);
    if(!source){
	fclose(file);
	return -1;
    }
    fread(source,1,(size_t)size, file);
    fclose(file);
    source[size]='\0';
    Lexer lexer = {0};
    lexer_init(&lexer,source);
    Token tokens[100];
    size_t i = 0;
    while(1){
	tokens[i] = next_token(&lexer);
	if(tokens[i].type == token_eof) break;
	i++;
    }
    printf("tokens[0] = %d (astept %d), data = %.*s\n",
       tokens[0].type,
       token_instruction,
       (int)tokens[0].data.size,
       tokens[0].data.p);

printf("tokens[1] = %d (astept %d), value = %u\n",
       tokens[1].type,
       token_immediate,
       tokens[1].valoare);

printf("tokens[2] = %d (astept %d) COMMA\n",
       tokens[2].type,
       token_comma);

printf("tokens[3] = %d (astept %d), data = %.*s\n",
       tokens[3].type,
       token_registru,
       (int)tokens[3].data.size,
       tokens[3].data.p);

printf("tokens[4] = %d (astept %d) NEWLINE\n",
       tokens[4].type,
       token_newline);

printf("tokens[5] = %d (astept %d), data = %.*s\n",
       tokens[5].type,
       token_instruction,
       (int)tokens[5].data.size,
       tokens[5].data.p);

printf("tokens[6] = %d (astept %d) NEWLINE\n",
       tokens[6].type,
       token_newline);

printf("tokens[7] = %d (astept %d) EOF\n",
       tokens[7].type,
       token_eof);
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
    free(source);
    return 0;
}
