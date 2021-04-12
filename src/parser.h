#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "object.h"

Object parse(Object tokens, Transition** syntax);

#endif
