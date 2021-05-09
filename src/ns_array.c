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
Object array_defineAll(Object env) {
  param_Array = primBuildTypeList(1, D_Array);
  param_ArrayInt = primBuildTypeList(2, D_Array, D_Int);
  param_ArrayIntAny = primBuildTypeList(3, D_Array, D_Int, D_Null);
  Object ns = hashNew();
  nsAddPrim(ns, "count", array_count);
  nsAddPrim(ns, "get", array_get);
  nsAddPrim(ns, "set", array_set);
  Object binding = bindingNew(identNew("array"), ns);
  return listNew(binding, env);
}

/*------------------------------------------------------------------*/
Object array_count(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_Array, args, thd);
  Object array = listGetFirst(args);
  Word nElems = arrayCount(array);
  return intNew(nElems);
}

/*------------------------------------------------------------------*/
Object array_get(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_ArrayInt, args, thd);
  Object array = listGetFirst(args);
  Object indexInt = listGetSecond(args);
  Word index = intGet(indexInt);
  return arrayGet(array, index, thd);
}

/*------------------------------------------------------------------*/
Object array_set(Thread* thd, Object args) {
  (void)thd;
  //printf("array_set param types = "); objShow(param_Arra
  primCheckArgs(param_ArrayIntAny, args, thd);
  Object array = listGetFirst(args);
  Object indexInt = listGetSecond(args);
  Object elem = listGetThird(args);
  Word index = intGet(indexInt);
  arraySet(array, index, elem, thd);
  return NOTHING;
}
