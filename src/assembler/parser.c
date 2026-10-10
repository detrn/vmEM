#include "assembler/parser.h"
#include "assembler/lexer.h"
#include "assembler/statement.h"
#include "assembler/token.h"

#ifndef REG_NUMBER
#define REG_NUMBER 15
#endif

#ifndef INSTRUCTION_NUMBER
#define INSTRUCTION_NUMBER 27
#endif
static void advance_parser(Parser* parser);

static int expect(Parser* parser, Token_type type){  
    if(parser->next.type != type){
	parser->err = 1; 
	return 0;
    }
    advance_parser(parser);
    return 1;
}

static void advance_parser(Parser* parser){
   parser->current = parser->next;
   parser->next = next_token(parser->lex);
}

static Instruction parse_instruction(Parser* parser){
    Opcode opcode = (Opcode) parser->current.valoare;
    printf("Opcode primit:%d == %d\n",opcode, op_ret);
    Instruction instruction = {.opcode = opcode};
    switch(opcode){ 
	//IMPORTANT:
	//Asta e un fel de contract, deoarece nu e evident deloc ceea ce se intampla. Current.valoare e primit de la lexer (token). Contractul 
	//impune faptul ca token .valoare are intregul ce corespunde cu intrare in ENUM (atat pentru registru cat si pentru instructiune)
	//Exista 26 de instructiuni si 15 registrii. 
	case op_halt:
	case op_nop:
	case op_ret:
	    printf("RET branch\n");
	    instruction.srcA.type = operand_none;
	    instruction.srcB.type = operand_none;
             break;
	case op_add:
        case op_sub:
        case op_and:
        case op_xor:
        case op_cmovle:
        case op_cmovl:
        case op_cmove:
        case op_cmovne:
        case op_cmovge:
        case op_cmovg:
	case op_rrmov: 
	    if(!expect(parser,token_registru)) break;
	    instruction.srcA.type = operand_register;
	    instruction.srcA.reg = parser->current.valoare;
	    if(!expect(parser,token_comma)) break;
	    if(!expect(parser,token_registru)) break;
	    instruction.srcB.type = operand_register;
	    instruction.srcB.reg = parser->current.valoare;
		break;
        case op_irmov:
	    if(!expect(parser,token_dollar)) break;
	    if(!expect(parser,token_number))
		break;
	    instruction.srcA.type = operand_immediate;
	    instruction.srcA.immediate = parser->current.valoare;
	    if(!expect(parser,token_comma))
		break;
	    if(!expect(parser,token_registru))
		break;
	    instruction.srcB.type = operand_register;
	    instruction.srcB.reg = parser->current.valoare;
	    break;
        case op_rmmov:
	    if(!expect(parser,token_registru))
		break;
	    
	    instruction.srcA.type = operand_register;
	    instruction.srcA.reg = parser->current.valoare; 
	    if(!expect(parser,token_comma))
		break;
	    if(parser->next.type == token_number){
		instruction.srcB.type = operand_memory; 
		instruction.srcB.memory = (MemoryOperand){.offset = parser->next.valoare};
		advance_parser(parser);
	    }
	    if(!expect(parser,token_lbracket))
		break;
	    
	    if(!expect(parser,token_registru))
		break;
	    
	    instruction.srcB.type = operand_memory;
	    instruction.srcB.memory.baza = parser->current.valoare;
	    if(!expect(parser,token_rbracket))
		break;
            break;
        case op_mrmov:
	    if(parser->next.type == token_number){
		instruction.srcA.type = operand_memory; 
		instruction.srcA.memory = (MemoryOperand){.offset = parser->next.valoare};
		advance_parser(parser);
	    }
	    if(!expect(parser,token_lbracket))
		break;
	    
	    if(!expect(parser,token_registru))
		break;
	    instruction.srcA.type = operand_memory;
	    instruction.srcA.memory.baza = parser->current.valoare;
	    if(!expect(parser,token_rbracket))
		break;
	    if(!expect(parser,token_comma)) break;
	    if(!expect(parser,token_registru)) break;
	    instruction.srcB.type = operand_register;
	    instruction.srcB.reg = parser->current.valoare;
            break;
        case op_jmp:
        case op_jle:
        case op_jl:
        case op_je:
        case op_jne:
        case op_jge:
        case op_jg:
        case op_call:{
	    int opExec = 0;
	    instruction.srcA.type = operand_none;
	    if(parser->next.type == token_identifier){
	    	instruction.srcB.type = operand_label;
		instruction.srcB.label = parser->next.data;
		opExec = 1;
		advance_parser(parser);
	    }
	    else{
		if(!expect(parser,token_dollar)) break;
		if(!expect(parser,token_number)) break;
		instruction.srcB.type = operand_immediate;
		instruction.srcB.immediate = parser->current.valoare;
		opExec = 1;
	    }
	    if(!opExec) parser-> err = 1;
	    break; 
	}
        case op_push:
	    //srcA pentru ca e sursa care se duce pe stiva 
	    if(!expect(parser,token_registru)) break;
	    instruction.srcA.type = operand_register; 
	    instruction.srcA.reg = parser->current.valoare;
            break;

        case op_pop:
	    if(!expect(parser,token_registru)) break;
	    //srcB ptc trebuia sa fie dest. La pop destinatia e registrul din instructiune
	    //deci incerc sa pastrez o ordine sa fie mai usor la encoder sau sa
	    //imit isaul mai bine zice
	    instruction.srcB.type = operand_register; 
	    instruction.srcB.reg = parser->current.valoare;
            break;
        default:
	    parser->err = 1;
            break;
    }
    if(parser->err) return instruction;
    if (parser->next.type == token_eof) advance_parser(parser);
    else expect(parser, token_newline);
    return instruction; 
}

void parser_init(Parser* parser, Lexer* lexer){
    parser->lex = lexer; 
    parser->current = next_token(parser->lex);
    parser->next = next_token(parser->lex);
    parser->err = 0; 
}
Statement parse(Parser* parser){
    //Primim token de la lexer. Trebuie sa construim un statement
    //Statementul poate contine: instructiune sau label 
    while(parser->current.type == token_newline) advance_parser(parser);
    if(parser->current.type == token_eof)return (Statement){.type = st_eof};
    if(parser->err == 1) return (Statement){.type = st_invalid};
    Statement statement; 
	switch(parser->current.type){
	    case token_instruction:{
		Instruction instruction;
		instruction = parse_instruction(parser);
		if(parser->err) return (Statement){.type = st_invalid};
		return (Statement){.type =st_instr,.instr = instruction}; 
		break;
	    }
	    case token_identifier:{
		String label = parser->current.data;
		if(!expect(parser,token_colon)) return (Statement){.type = st_invalid};
	        if (!expect(parser, token_newline)) return (Statement){.type = st_invalid};
		statement =(Statement){.type = st_label, .label = label};
		return statement; 
	    }
	    default:
		statement =(Statement){.type = st_invalid};
		return statement;
	}
} 

static void print_operand(const Operand* op){
    switch(op->type){
	case operand_label:
	    printf("Label data:%.*s\n",(int)op->label.size,op->label.p);
	    break;
	case operand_none:
	    printf("Nu exista operand\n");
	    break;
	case operand_register:{
	    const char *registrii[REG_NUMBER] = {
    "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
    "R8", "R9", "R10", "R11", "R12", "R13", "R14"
};
	    printf("Valoare pentru registru DEBUG:%d\n",op->reg);
	    printf("Registru: %s\n",registrii[op->reg]);
	    break;
	    }
	case operand_immediate:
	    printf("Immediate: %d\n",op->immediate);
	    break;
	case operand_memory:
	    printf("Baza: %d, offset: %d\n",op->memory.baza,op->memory.offset);
	    break;
	default:
	    printf("Operand invalid (eroare)\n");
	    break;
	    
    }
}


static void print_statement(const Statement* statement){
    switch(statement->type){
	case st_instr:{
	    const char *instructions[INSTRUCTION_NUMBER] = { "HALT", "NOP", "RRMOVW", "IRMOVW", "RMMOVW", "MRMOVW", "ADD", "SUBW", "ANDW", "XORW", "JMP", "JLE", "JL", "JE", "JNE", "JGE", "JG","CMOVLE","CMOVL","CMOVE","CMOVNE","CMOVGE","CMOVG","CALL","RET","PUSHW","POPW"};
	    printf("Instructiune:%s\n",instructions[statement->instr.opcode]);
	    print_operand(&statement->instr.srcA);
	    print_operand(&statement->instr.srcB);
	    break;
	}
	case st_label:
	    printf("Label name:%.*s\n",(int)statement->label.size,statement->label.p);
	    break;
	case st_eof:
	    printf("End of file\n");
	    break;
	case st_invalid:
	case st_directive:
	default:
	    printf("Statement error\n");
	    break;
    }
}

Statement parser_debug_next(Parser* parser){
    lexer_debug(parser->lex,&parser->current);
    Statement statement = parse(parser);
    print_statement(&statement);
    return statement; 

}
