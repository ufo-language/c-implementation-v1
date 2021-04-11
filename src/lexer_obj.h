#ifndef LEXER_OBJ_H
#define LEXER_OBJ_H

#include "object.h"

/* The lexer.{h,c} files are independent of the specialized data types
   used in UFO, such as Object. This file is the interface between the
   raw lexer and the UFO Object system. */

/* Takes a string and returns a list of tokens. */
Object lex(Object string);

#endif
