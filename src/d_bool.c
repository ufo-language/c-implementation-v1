#include <stdio.h>

#include "d_bool.h"
#include "defines.h"

Object boolNew(bool b) {
  Object bln = objAlloc(D_Bool, 1);
  objSetData(bln, 0, b);
  return bln;
}

bool boolEqual(Object bln, Object other) {
  return boolGet(bln) == boolGet(other);
}

bool boolGet(Object bln) {
  return objGetData(bln, 0);
}

void boolShow(Object bln, FILE* stream) {
  fputs(boolGet(bln) ? BOOL_TRUE_STRING : BOOL_FALSE_STRING, stream);
}
