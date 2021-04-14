#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "object.h"

typedef Object (*Parser)(Object tokens);

Object parseCharString(char* input, Parser parser);
Object parse(Parser parser, Object tokens);

/* primitive */
Object p_spot(Object tokenList, TokenType tokenType, Object value);

/* combinators */
Object p_maybe(Object tokens, Parser parser);
Object p_oneOf(Object tokens, Parser* parsers);
Object p_seq(Object tokens, Parser* parsers);
Object p_some(Object tokens, Parser parser, int min);

Object p_reserved(Object tokens, Object reservedString);

/* object parsers */
Object p_bool(Object tokens);
Object p_int(Object tokens);
Object p_real(Object tokens);
Object p_string(Object tokens);
Object p_symbol(Object tokens);

Object p_number(Object tokens);
Object p_object(Object tokens);

/* expression parsers */
Object p_ident(Object tokens);
Object p_if(Object tokens);

/* reserved word parsers */
Object p_end(Object tokens);

#endif
