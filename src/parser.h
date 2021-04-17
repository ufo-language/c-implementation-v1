#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "object.h"
#include "thread.h"

typedef Object (*Parser)(Thread* thd, Object tokens);

Object parse(Thread* thd, Parser parser, Object tokens);

/* TODO most of these declarations should not be in this header file */

/* primitive */
Object p_spot(Object tokenList, TokenType tokenType);
Object p_spotReserved(Object tokenList, char* word);
Object p_spotSpecial(Object tokenList, char* word);

/* combinators */
Object p_ignore(Thread* thd, Object tokens, Parser parser);
Object p_maybe(Thread* thd, Object tokens, Parser parser);
Object p_oneOf(Thread* thd, Object tokens, Parser* parsers);
Object p_seq(Thread* thd, Object tokens, Parser* parsers);
Object p_some(Thread* thd, Object tokens, Parser parser, int min);

/* object parsers */
Object p_bool(Thread* thd, Object tokens);
Object p_int(Thread* thd, Object tokens);
Object p_real(Thread* thd, Object tokens);
Object p_string(Thread* thd, Object tokens);
Object p_symbol(Thread* thd, Object tokens);

//Object p_number(Thread* thd, Object tokens);
Object p_object(Thread* thd, Object tokens);

/* reserved word parsers */
Object p_end(Thread* thd, Object tokens);

/* expression parsers */
Object p_ident(Thread* thd, Object tokens);
Object p_if(Thread* thd, Object tokens);

Object p_any(Thread* thd, Object tokens);

#endif
