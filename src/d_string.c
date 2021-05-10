#include <stdio.h>
#include <string.h>

#include "d_array.h"
#include "d_int.h"
#include "d_string.h"
#include "defines.h"
#include "hash.h"
#include "object.h"
#include "thread.h"

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
    if (stringGetChar_unsafe(string, n) != stringGetChar_unsafe(other, n)) {
      return false;
    }
  }
  return true;
}

/*------------------------------------------------------------------*/
bool stringEqualsChars(Object string, char* chars) {
  Word len = stringCount(string);
  for (Word n=0; n<len; n++) {
    if (stringGetChar_unsafe(string, n) != chars[n]) {
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
    hashCode = hashRotateLeft(hashCode) ^ stringGetChar_unsafe(str, n);
  }
  return hashCode;
}

/*------------------------------------------------------------------*/
Object stringNew(char* str) {
  int len = strlen(str);
  Word nWords = len / sizeof(Word) + 2;
  Object string = objAlloc(D_String, nWords);
  objSetData(string, 0, len);
  /* going '<= len' includes the null terminator */
  for (int n=0; n<=len; n++) {
    stringSetChar_unsafe(string, n, str[n]);
  }
  return string;
}

/*------------------------------------------------------------------*/
char stringGetChar(Object string, Word index, Thread* thd) {
  int nChars = stringCount(string);
  if (index >= nChars) {
    Object exn = arrayNew(3);
    arraySet_unsafe(exn, 0, intNew(index));
    arraySet_unsafe(exn, 1, intNew(stringCount(string)));
    arraySet_unsafe(exn, 2, string);
    threadThrowException(thd, "Error", "String index out of bounds", exn);
  }
  return stringGetChar_unsafe(string, index);
}

/*------------------------------------------------------------------*/
char stringGetChar_unsafe(Object string, Word index) {
  Word word = objGetData(string, index / 2 + 1);
  if (index % 2 == 0) {
    return word & 0xFF;
  }
  return (word >> 8);
}

/*------------------------------------------------------------------*/
void stringSetChar(Object string, Word index, char c, Thread* thd) {
  int nChars = stringCount(string);
  if (index >= nChars) {
    Object exn = arrayNew(3);
    arraySet_unsafe(exn, 0, intNew(index));
    arraySet_unsafe(exn, 1, intNew(stringCount(string)));
    arraySet_unsafe(exn, 2, string);
    threadThrowException(thd, "Error", "String index out of bounds", exn);
  }
  stringSetChar_unsafe(string, index, c);
}

/*------------------------------------------------------------------*/
void stringSetChar_unsafe(Object string, Word index, char c) {
  Word addr = index / 2 + 1;
  Word word = objGetData(string, addr);
  if (index % 2 == 0) {
    word = (word & 0xFF00) | c;
  }
  else {
    word = (word & 0xFF) | (c << 8);
  }
  objSetData(string, addr, word);
}

/*------------------------------------------------------------------*/
void stringDisp(Object string, FILE* stream) {
  stringUnescapify(string, stream);
}

/*------------------------------------------------------------------*/
void stringShow(Object string, FILE* stream) {
  fputc('"', stream);
  stringEscapify(string, stream);
  fputc('"', stream);
}

/*------------------------------------------------------------------*/
void stringEscapify(Object string, FILE* stream) {
  Word len = stringCount(string);
  for (Word n=0; n<len; n++) {
    char c = stringGetChar_unsafe(string, n);
    switch (c) {
      case '"' : fprintf(stream, "\\\""); break;
      case '\\': fprintf(stream, "\\\\"); break;
      case '\b': fprintf(stream, "\\b"); break;
      case '\f': fprintf(stream, "\\f"); break;
      case '\n': fprintf(stream, "\\n"); break;
      case '\r': fprintf(stream, "\\r"); break;
      case '\t': fprintf(stream, "\\t"); break;
      default:
        fputc(c, stream);
    }
  }
}

/*------------------------------------------------------------------*/
void stringUnescapify(Object string, FILE* stream) {
  bool escaped = false;
  Word len = stringCount(string);
  for (Word n=0; n<len; n++) {
    char c = stringGetChar_unsafe(string, n);
    if (escaped) {
      switch(c) {
        case '\"': fputc('\"', stream); break;
        case '\\': fputc('\\', stream); break;
        case 'b' : fputc('\b', stream); break;
        case 'f' : fputc('\f', stream); break;
        case 'n' : fputc('\n', stream); break;
        case 'r' : fputc('\r', stream); break;
        case 't' : fputc('\t', stream); break;
        default:
          fputc(c, stream);
      }
      escaped = false;
    }
    else if (c == '\\') {
      escaped = true;
    }
    else {
      fputc(c, stream);
    }
  }
}
