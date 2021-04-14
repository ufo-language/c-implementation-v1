#ifndef D_STRING_H
#define D_STRING_H

#include <stdio.h>

#include "defines.h"
#include "object.h"

Object stringNew(char* str);

Word stringCount(Object string);
void stringDisp(Object string, FILE* stream);
bool stringEquals(Object string, Object other);
bool stringEqualsChars(Object string, char* chars);
Word stringHash(Object string);
Word stringHash_aux(Object string);
char stringGetChar(Object string, Word offset);
void stringSetChar(Object string, Word offset, char c);
void stringShow(Object string, FILE* stream);

#endif
