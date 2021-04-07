#include <string.h>

#include "d_string.h"
#include "d_symbol.h"
#include "hash.h"
#include "object.h"
#include "thread.h"

void objSetType(Object obj, ObjType type);

bool symbolEqual(Object symbol, Object obj) {
  /* treat symbol as a string and compare */
  return stringEqual(symbol, obj);
}

Word symbolHash(Object symbol) {
  return stringHash_aux(symbol) ^ hashPrimes(objGetType(symbol));
}

Object symbolNew(char* str) {
  Object symbol = stringNew(str);
  objSetType(symbol, D_Symbol);
  return symbol;
}

void symbolShow(Object symbol, FILE* stream) {
  stringDisp(symbol, stream);
}
