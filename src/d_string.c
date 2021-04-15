#include <stdio.h>
#include <string.h>

#include "d_string.h"
#include "defines.h"
#include "hash.h"
#include "object.h"

/*------------------------------------------------------------------*/
Word stringCount(Object string) {
  return objGetData(string, 0);
}

/*------------------------------------------------------------------*/
bool stringEquals(Object string, Object other) {
  Word len1 = stringCount(string);
  Word len2 = stringCount(other);
  if (len1 != len2) {
    return false;
  }
  for (Word n=0; n<len1; n++) {
    if (stringGetChar(string, n) != stringGetChar(other, n)) {
      return false;
    }
  }
  return true;
}

/*------------------------------------------------------------------*/
void objShow(Object obj, FILE* file);
bool stringEqualsChars(Object string, char* chars) {
  Word len = stringCount(string);
  for (Word n=0; n<len; n++) {
    if (stringGetChar(string, n) != chars[n]) {
      return false;
    }
  }
  return true;
}

/*------------------------------------------------------------------*/
Word stringHash(Object str) {
 return stringHash_aux(str) ^ hashPrimes(objGetType(str));
}

/*------------------------------------------------------------------*/
Word stringHash_aux(Object str) {
  Word hashCode = 0;
  Word strLen = stringCount(str);
  for (Word n=0; n<strLen; n++) {
    hashCode = hashRotateLeft(hashCode) ^ stringGetChar(str, n);
  }
  return hashCode;
}

/*------------------------------------------------------------------*/
Object stringNew(char* str) {
  int len = strlen(str);
  Word nWords = (len + 1) / sizeof(Word);
  Object string = objAlloc(D_String, nWords);
  objSetData(string, 0, len);
  /* going <= len includes the null terminator */
  for (int n=0; n<=len; n++) {
    stringSetChar(string, n, str[n]);
  }
  return string;
}

/*------------------------------------------------------------------*/
char stringGetChar(Object string, Word offset) {
  Word word = objGetData(string, 1 + offset / 2);
  if (offset % 2 == 0) {
    return word & 0xFF;
  }
  return (word >> 8);
}

/*------------------------------------------------------------------*/
void stringSetChar(Object string, Word offset, char c) {
  Word word = objGetData(string, 1 + offset / 2);
  if (offset % 2 == 0) {
    word = (word & 0xFF00) | c;
  }
  else {
    word = (word & 0xFF) | (c << 8);
  }
  objSetData(string, 1 + offset / 2, word);
}

/*------------------------------------------------------------------*/
void stringDisp(Object string, FILE* stream) {
  Word len = stringCount(string);
  for (Word n=0; n<len; n++) {
    char c = stringGetChar(string, n);
    fputc(c, stream);
  }
}

/*------------------------------------------------------------------*/
void stringShow(Object string, FILE* stream) {
  fputc('"', stream);
  stringDisp(string, stream);
  fputc('"', stream);
}
