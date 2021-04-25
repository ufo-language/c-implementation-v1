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
void stringEscapify(Object string, FILE* stream);
Word stringHash(Object string);
Word stringHash_aux(Object string);
char stringGetChar(Object string, Word offset);
void stringSetChar(Object string, Word offset, char c);
void stringShow(Object string, FILE* stream);
void stringUnescapify(Object string, FILE* stream);

#endif
