#ifndef D_STRING_H
#define D_STRING_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

Object stringNew(char* str);

Object stringAlloc(Word nChars);
Word stringCount(Object string);
void stringDisp(Object string, FILE* stream);
bool stringEquals(Object string, Object other);
bool stringEqualsChars(Object string, char* chars);
void stringEscapify(Object string, FILE* stream);
Word stringHash(Object string);
Word stringHash_aux(Object string);
char stringGetChar(Object string, Word offset, Thread* thd);
char stringGetChar_unsafe(Object string, Word offset);
void stringSetChar(Object string, Word offset, char c, Thread* thd);
void stringSetChar_unsafe(Object string, Word offset, char c);
void stringShow(Object string, FILE* stream);
void stringShowChars(Object string, FILE* stream);
Object stringSubstring(Object string, Word start, Word end);
void stringToCharStar(Object string, char* buf, Word bufSize);
void stringUnescapify(Object string, FILE* stream);

#endif
