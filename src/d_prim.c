#include <stdio.h>

#include "d_prim.h"
#include "defines.h"

/*------------------------------------------------------------------*/
Object primNew(PrimFunc func) {
  Object prim = objAlloc(D_Prim, PRIM_OBJ_SIZE);
  union {
    Word w[2];
    PrimFunc f;
  } u;
  u.f = func;
  objSetData(prim, 0, u.w[0]);
  objSetData(prim, 1, u.w[1]);
  return prim;
}

/*------------------------------------------------------------------*/
PrimFunc primGet(Object prim) {
  union {
    Word w[2];
    PrimFunc f;
  } u;
  u.w[0] = objGetData(prim, 0);
  u.w[1] = objGetData(prim, 1);
  return u.f;
}

/*------------------------------------------------------------------*/
void primShow(Object prim, FILE* stream) {
  fprintf(stream, "prim@%p", (void*)primGet(prim));
}
