#include <stdarg.h>

#include "d_array.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_queue.h"
#include "d_symbol.h"
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
Object primBuildTypeList(int nArgs, ...) {
  va_list argList;
  va_start(argList, nArgs);
  Object q = queueNew();
  for (int n=0; n<nArgs; n++) {
    ObjType objType = va_arg(argList, ObjType);
    Object typeSym;
    if (objType == D_Null) {
      typeSym = symbolNew("Any");
    }
    else {
      typeSym = symbolNew(ObjTypeNames[objType]);
    }
    queueEnq(q, typeSym);
  }
  va_end(argList);
  return registerGlobal(queueAsList(q));
}

/*------------------------------------------------------------------*/
void primArgError(Word n, Object paramTypes, Object args, Thread* thd) {
    Object exn = arrayN(3, intNew(n), paramTypes, args);
    threadThrowException(thd, "ArgumentError", "parameter/argument mismatch", exn);
}

/*------------------------------------------------------------------*/
void primCheckArgs2(Object paramTypes, Object args, Object** argVars, Thread* thd) {
  int n = primCheckArgs2_unsafe(paramTypes, args, argVars);
  if (n > -1) {
    primArgError((Word)n, paramTypes, args, thd);
  }
}

/*------------------------------------------------------------------*/
int primCheckArgs2_unsafe(Object paramTypes, Object args, Object** argVars) {
  bool error = false;
  Word n = 0;
  while (!error) {
    n++;
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
    Object* argVar = *argVars;
    if (objHasType(arg, paramType)) {
      *argVar = arg;
    }
    else {
      error = true;
      break;
    }
    paramTypes = listGetRest(paramTypes);
    args = listGetRest(args);
    argVars++;
  }
  return error ? n : -1;
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
