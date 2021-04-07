#include <stdio.h>

#include "d_int.h"
#include "defines.h"
#include "hash.h"

Object intNew(int i) {
  Object integer = objAlloc(D_Int, 1);
  objSetData(integer, 0, i);
  return integer;
}

bool intEqual(Object integer, Object other) {
  return intGet(integer) == intGet(other);
}

int intGet(Object integer) {
  return objGetData(integer, 0);
}

int intHash(Object integer) {
  return intGet(integer) ^ hashPrimes(objGetType(integer));
}

void intShow(Object integer, FILE* stream) {
  fprintf(stream, "%d", intGet(integer));
}

