#ifndef PARSER_H 
#define PARSER_H 
#include "lexer.h"
#include "token.h"
#include "statement.h"

/*
 * The parser structure has in its component the lexer, current, next and err. 
 * Current is the token that is currently processed and next is a look ahead 
 * Error signals that the parser has encountered an error 
 */

typedef struct{
    Lexer* lex; 
    Token current; 
    Token next;
    int err; 
}Parser;


/*
 * It's similar to next_token. It sequently takes in tokens
 * from the lexer and returns a statement. 
 * The function returns st_eof at the end of the processing. 
 * In case of an error it will update .err and return st_invalid. 
 */

Statement parse(Parser* parser);

/*
 * Assigns the first token produced by the lexer to .current and the second to .next. 
 * The lexer must already be initialized before calling the parser_init()
 * TO DO:
 * Schimba dependenta si fa ca assembler sa foloseasca toate initurile si sa proceseze 
 * source fileul 
 */
void parser_init(Parser* parser, Lexer* lexer);

/*
 * Debug function that prints the lexer_debug output aswell as the data for the 
 * statement that is used produced by parse. 
 * Use it only for debugging as it consumes the statement 
 *
 */
Statement parser_debug_next(Parser* parser); 

#if defined DEBUG_PARSER || DEBUG 
    #define DEBUG_PARSER_PRINT(parser) parser_debug_next(parser)
    
#else 
    #define DEBUG_PARSER_PRINT(parser) ((void)0)
#endif
#endif 


