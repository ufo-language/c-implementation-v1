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
Object any_type(Thread* thd, Object args);

static Object param_Any;
static Object param_AnyAny;

/*------------------------------------------------------------------*/
Object any_defineAll(Object env) {
  param_Any = primBuildTypeList(1, D_Null);
  param_AnyAny = primBuildTypeList(2, D_Null, D_Null);
  Object ns = hashNew();
  nsAddPrim(ns, "freeVars", any_freeVars);
  nsAddPrim(ns, "hashCode", any_hashCode);
  nsAddPrim(ns, "match", any_match);
  nsAddPrim(ns, "type", any_type);
  Object binding = bindingNew(identNew("any"), ns);
  return listNew(binding, env);
}

/*------------------------------------------------------------------*/
Object any_freeVars(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_Any, args, thd);
  Object freeVarSet = setNew();
  Object arg = listGetFirst(args);
  objFreeVars(arg, freeVarSet);
  return freeVarSet;
}

/*------------------------------------------------------------------*/
Object any_hashCode(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_Any, args, thd);
  Object arg = listGetFirst(args);
  Word hashCode = objHashCode(arg);
  return intNew(hashCode);
}

/*------------------------------------------------------------------*/
Object any_match(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_AnyAny, args, thd);
  return objMatch(listGetFirst(args), listGetSecond(args), EMPTY_LIST);
}

/*------------------------------------------------------------------*/
Object any_type(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_Any, args, thd);
  Object arg = listGetFirst(args);
  ObjType objType = objGetType(arg);
  char* typeName = ObjTypeNames[objType];
  Object typeSym = symbolNew(typeName);
  return typeSym;
}
