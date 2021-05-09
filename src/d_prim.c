#include "d_list.h"
#include "d_prim.h"
#include "defines.h"
#include "delegate.h"
#include "globals.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object primApply(Object prim, Object argList, Thread* thd) {
  PrimFunc primFunc = primGet(prim);
  Object res = primFunc(thd, argList);
  return res;
}

/*------------------------------------------------------------------*/
void primCheckArgs(Object paramTypes, Object args, Thread* thd) {
  bool error = false;
  while (!error) {
    if (listIsEmpty(paramTypes)) {
      error = !listIsEmpty(args);
      break;
    }
    if (listIsEmpty(args)) {
      error = true;
      break;
    }
    Object paramType = listGetFirst(paramTypes);
    Object arg = listGetFirst(args);
    if (!objHasType(arg, paramType)) {
      error = true;
      break;
    }
    paramTypes = listGetRest(paramTypes);
    args = listGetRest(args);
  }
  if (error) {
    threadThrowException(thd, "ArgumentError", "parameter/argument mismatch", listNew(paramTypes, args));
  }
}

/*------------------------------------------------------------------*/
Object primNew(PrimFunc func) {
  Object prim = objAlloc(D_Prim, PRIM_OBJ_SIZE);
  union {
    Word w[PRIM_OBJ_SIZE];
    PrimFunc f;
  } u;
  u.f = func;
  for (int n=0; n<(int)PRIM_OBJ_SIZE; n++) {
    objSetData(prim, n, u.w[n]);
  }
  return prim;
}

/*------------------------------------------------------------------*/
Object primMacroNew(PrimFunc func) {
  Object prim = objAlloc(D_PrimMacro, PRIM_OBJ_SIZE);
  union {
    Word w[PRIM_OBJ_SIZE];
    PrimFunc f;
  } u;
  u.f = func;
  for (int n=0; n<(int)PRIM_OBJ_SIZE; n++) {
    objSetData(prim, n, u.w[n]);
  }
  return prim;
}

/*------------------------------------------------------------------*/
PrimFunc primGet(Object prim) {
  union {
    Word w[PRIM_OBJ_SIZE];
    PrimFunc f;
  } u;
  for (int n=0; n<(int)PRIM_OBJ_SIZE; n++) {
    u.w[n] = objGetData(prim, n);
  }
  return u.f;
}

/*------------------------------------------------------------------*/
void primShow(Object prim, FILE* stream) {
  union {
    PrimFunc p;
    void* v;
  } u;
  u.p = primGet(prim);
  fprintf(stream, "prim@%p", u.v);
}
