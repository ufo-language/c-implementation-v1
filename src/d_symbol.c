#include <string.h>

#include "d_string.h"
#include "d_symbol.h"
#include "hash.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
bool symbolEquals(Object symbol, Object obj) {
  /* treat symbol as a string and compare */
  return stringEquals(symbol, obj);
}

/*------------------------------------------------------------------*/
Word symbolHash(Object symbol) {
  return stringHash_aux(symbol) ^ hashPrimes(objGetType(symbol));
}

/*------------------------------------------------------------------*/
bool symbolHasName(Object symbol, char* name) {
  Word nChars = stringCount(symbol);
  int n = 0;
  while (name && n < nChars) {
    if (stringGetChar(symbol, n) != *name) {
      return false;
    }
    n++;
    name++;
  }
  return n == nChars;
}

/*------------------------------------------------------------------*/
Object symbolNew(char* str) {
  /* a symbol is a string with its ObjType set to D_Symbol */
  Object symbol = stringNew(str);
  objSetType(symbol, D_Symbol);
  return symbol;
}

/*------------------------------------------------------------------*/
void symbolShow(Object symbol, FILE* stream) {
  stringDisp(symbol, stream);
}
