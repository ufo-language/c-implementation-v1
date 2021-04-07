#ifndef D_STRING_H
#define D_STRING_H

#include <stdio.h>

#include "defines.h"
#include "object.h"

Object stringNew(char* str);

void stringDisp(Object string, FILE* stream);
bool stringEqual(Object string, Object other);
Word stringHash(Object string);
Word stringHash_aux(Object string);
char stringGetChar(Object string, Word offset);
void stringSetChar(Object string, Word offset, char c);
Word stringCount(Object string);
void stringShow(Object string, FILE* stream);

#endif
