#include <stdio.h>

#include "d_array.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object array_count(Thread* thd, Object args);
Object array_get(Thread* thd, Object args);
Object array_set(Thread* thd, Object args);

static Object param_Array;
static Object param_ArrayInt;
static Object param_ArrayIntAny;

/*------------------------------------------------------------------*/
void array_defineAll(Object env, Thread* thd) {
  char* nsName = "array";
  param_Array = primBuildTypeList(1, D_Array);
  param_ArrayInt = primBuildTypeList(2, D_Array, D_Int);
  param_ArrayIntAny = primBuildTypeList(3, D_Array, D_Int, D_Null);
  Object ns = hashNew();
  nsAddPrim(ns, "count", array_count, thd);
  nsAddPrim(ns, "get", array_get, thd);
  nsAddPrim(ns, "set", array_set, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
Object array_count(Thread* thd, Object args) {
  Object array;
  Object* argAry[] = {&array};
  primCheckArgs(param_Array, args, argAry, thd);
  Word nElems = arrayCount(array);
  return intNew(nElems);
}

/*------------------------------------------------------------------*/
Object array_get(Thread* thd, Object args) {
  Object array, indexInt;
  Object* argAry[] = {&array, &indexInt};
  primCheckArgs(param_ArrayInt, args, argAry, thd);
  Word index = intGet(indexInt);
  return arrayGet(array, index, thd);
}

/*------------------------------------------------------------------*/
Object array_set(Thread* thd, Object args) {
  Object array, indexInt, elem;
  Object* argAry[] = {&array, &indexInt, &elem};
  primCheckArgs(param_ArrayIntAny, args, argAry, thd);
  Word index = intGet(indexInt);
  arraySet(array, index, elem, thd);
  return array;
}
