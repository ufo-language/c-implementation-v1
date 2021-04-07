#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "object.h"

Address parse(char* inputString, Transition** syntax);

#endif
