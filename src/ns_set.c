#include <stdio.h>

#include "d_binding.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_set.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object set_add(Thread* thd, Object args);
Object set_contains(Thread* thd, Object args);
Object set_new(Thread* thd, Object args);
Object set_remove(Thread* thd, Object args);

static Object param_SetAny;

/*------------------------------------------------------------------*/
void set_defineAll(Object env, Thread* thd) {
  char* nsName = "set";
  param_SetAny = primBuildTypeList(2, D_Set, D_Null);
  Object ns = hashNew();
  nsAddPrim(ns, "add", set_add, thd);
  nsAddPrim(ns, "contains", set_contains, thd);
  nsAddPrim(ns, "new", set_new, thd);
  nsAddPrim(ns, "remove", set_remove, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
Object set_add(Thread* thd, Object args) {
  Object set, elem;
  Object* argAry[] = {&set, &elem};
  primCheckArgs(param_SetAny, args, argAry, thd);
  setAddElem(set, elem, thd);
  return set;
}

/*------------------------------------------------------------------*/
Object set_contains(Thread* thd, Object args) {
  Object set, elem;
  Object* argAry[] = {&set, &elem};
  primCheckArgs(param_SetAny, args, argAry, thd);
  return setContains(set, elem, thd) ? TRUE : FALSE;
}

/*------------------------------------------------------------------*/
Object set_new(Thread* thd, Object args) {
  Object set = setNew();
  while (!listIsEmpty(args)) {
    Object elem = listGetFirst(args);
    setAddElem(set, elem, thd);
    args = listGetRest(args);
  }
  return set;
}

/*------------------------------------------------------------------*/
Object set_remove(Thread* thd, Object args) {
  Object set, elem;
  Object* argAry[] = {&set, &elem};
  primCheckArgs(param_SetAny, args, argAry, thd);
  setRemoveElem(set, elem, thd);
  return set;
}
