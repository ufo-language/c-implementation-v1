#include <stdio.h>

#include "d_tuple.h"
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

Object tuple_count(Thread* thd, Object args);
Object tuple_get(Thread* thd, Object args);
Object tuple_set(Thread* thd, Object args);

static Object param_Tuple;
static Object param_TupleInt;

/*------------------------------------------------------------------*/
void tuple_defineAll(Object env, Thread* thd) {
  char* nsName = "tuple";
  param_Tuple = primBuildTypeList(1, D_Tuple);
  param_TupleInt = primBuildTypeList(2, D_Tuple, D_Int);
  Object ns = hashNew();
  nsAddPrim(ns, "count", tuple_count, thd);
  nsAddPrim(ns, "get", tuple_get, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
Object tuple_count(Thread* thd, Object args) {
  Object tuple;
  Object* argAry[] = {&tuple};
  primCheckArgs2(param_Tuple, args, argAry, thd);
  Word nElems = tupleCount(tuple);
  return intNew(nElems);
}

/*------------------------------------------------------------------*/
Object tuple_get(Thread* thd, Object args) {
  Object tuple;
  Object* argAry[] = {&tuple};
  primCheckArgs2(param_TupleInt, args, argAry, thd);
  Object indexInt = listGetSecond(args);
  Word index = intGet(indexInt);
  return tupleGet(tuple, index, thd);
}
