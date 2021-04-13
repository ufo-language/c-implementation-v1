#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "object.h"

typedef Object (*Parser)(Object tokens);

Object parse(Object tokens, Transition** syntax);

/* primitive */
Object p_spot(Object tokenList, TokenType tokenType, Object value);

/* combinators */
Object p_maybe(Object tokens, Parser parser);
Object p_oneOf(Object tokens, Parser* parsers);
Object p_seq(Object tokens, Parser* parsers);
Object p_some(Object tokens, Parser parser, int min);

/* object parsers */
Object p_bool(Object tokens);
Object p_int(Object tokens);
Object p_real(Object tokens);
Object p_number(Object tokens);
Object p_string(Object tokens);
Object p_object(Object tokens);


#endif
