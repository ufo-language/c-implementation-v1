#include <stdio.h>

#include "d_binding.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_set.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object any_freeVars(Thread* thd, Object args);
Object any_hashCode(Thread* thd, Object args);
Object any_match(Thread* thd, Object args);

static Object param_Any;
static Object param_AnyAny;

/*------------------------------------------------------------------*/
void any_defineAll(Object env) {
  char* nsName = "any";
  param_Any = primBuildTypeList(1, D_Null);
  param_AnyAny = primBuildTypeList(2, D_Null, D_Null);
  Object ns = hashNew();
  nsAddPrim(ns, "freeVars", any_freeVars);
  nsAddPrim(ns, "hashCode", any_hashCode);
  nsAddPrim(ns, "match", any_match);
  hashPut(env, identNew(nsName), ns);
}

/*------------------------------------------------------------------*/
Object any_freeVars(Thread* thd, Object args) {
  Object arg;
  Object* argAry[] = {&arg};
  primCheckArgs2(param_Any, args, argAry, thd);
  Object freeVarSet = setNew();
  objFreeVars(arg, freeVarSet);
  return freeVarSet;
}

/*------------------------------------------------------------------*/
Object any_hashCode(Thread* thd, Object args) {
  Object arg;
  Object* argAry[] = {&arg};
  primCheckArgs2(param_Any, args, argAry, thd);
  Word hashCode = objHashCode(arg);
  return intNew(hashCode);
}

/*------------------------------------------------------------------*/
Object any_match(Thread* thd, Object args) {
  Object arg1, arg2;
  Object* argAry[] = {&arg1, &arg2};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  return objMatch(arg1, arg1, EMPTY_LIST);
}
