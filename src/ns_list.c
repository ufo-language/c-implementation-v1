#include <stdio.h>

#include "d_binding.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_queue.h"
#include "d_set.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_app.h"
#include "e_ident.h"
#include "eval.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object list_accept(Thread* thd, Object args);
Object list_count(Thread* thd, Object args);
Object list_drop(Thread* thd, Object args);
Object list_first(Thread* thd, Object args);
Object list_keys(Thread* thd, Object args);
Object list_map(Thread* thd, Object args);
Object list_reject(Thread* thd, Object args);
Object list_rest(Thread* thd, Object args);
Object list_reverse(Thread* thd, Object args);
Object list_setFirst(Thread* thd, Object args);
Object list_setRest(Thread* thd, Object args);
Object list_take(Thread* thd, Object args);

static Object param_List;
static Object param_ListAny;
static Object param_ListInt;

/*------------------------------------------------------------------*/
void list_defineAll(Object env, Thread* thd) {
  char* nsName = "list";
  param_List = primBuildTypeList(1, D_List);
  param_ListAny = primBuildTypeList(2, D_List, D_Null);
  param_ListInt = primBuildTypeList(2, D_List, D_Int);
  Object ns = hashNew();
  nsAddPrim(ns, "accept", list_accept, thd);
  nsAddPrim(ns, "drop", list_drop, thd);
  nsAddPrim(ns, "count", list_count, thd);
  nsAddPrim(ns, "first", list_first, thd);
  nsAddPrim(ns, "keys", list_keys, thd);
  nsAddPrim(ns, "map", list_map, thd);
  nsAddPrim(ns, "reject", list_reject, thd);
  nsAddPrim(ns, "rest", list_rest, thd);
  nsAddPrim(ns, "setFirst", list_setFirst, thd);
  nsAddPrim(ns, "setRest", list_setRest, thd);
  nsAddPrim(ns, "reverse", list_reverse, thd);
  nsAddPrim(ns, "take", list_take, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
Object list_accept(Thread* thd, Object args) {
  Object list, fun;
  Object* argAry[] = {&list, &fun};
  primCheckArgs(param_ListAny, args, argAry, thd);
  Object funVal = eval(fun, thd);
  Object q = queueNew();
  while (!listIsEmpty(list)) {
    Object elem = listGetFirst(list);
    Object app = appNew(funVal, listNew(elem, EMPTY_LIST));
    Object val = eval(app, thd);
    if (objBoolValue(val)) {
      queueEnq(q, elem);
    }
    list = listGetRest(list);
    if (objGetType(list) != D_List) {
      list = listNew(list, EMPTY_LIST);
    }
  }
  return queueAsList(q);
}

/*------------------------------------------------------------------*/
Object list_count(Thread* thd, Object args) {
  Object list;
  Object* objAry[] = {&list};
  primCheckArgs(param_List, args, objAry, thd);
  Word nElems = listCount(list);
  return intNew(nElems);
}

/*------------------------------------------------------------------*/
Object list_drop(Thread* thd, Object args) {
  Object list, nElemsInt;
  Object* argAry[] = {&list, &nElemsInt};
  primCheckArgs(param_ListInt, args, argAry, thd);
  Word nElems = intGet(nElemsInt);
  for (int n=0; n<nElems; n++) {
    list = listGetRest(list);
  }
  return list;
}

/*------------------------------------------------------------------*/
Object list_first(Thread* thd, Object args) {
  Object list;
  Object* argAry[] = {&list};
  primCheckArgs(param_List, args, argAry, thd);
  return listGetFirst(list);
}

/*------------------------------------------------------------------*/
static void list_keys_callback(Object keySet, Object elem, Thread* thd) {
  if (D_Binding == objGetType(elem)) {
    setAddElem(keySet, bindingGetLhs(elem), thd);
  }
}

/*------------------------------------------------------------------*/
Object list_keys(Thread* thd, Object args) {
  Object list;
  Object* argAry[] = {&list};
  primCheckArgs(param_List, args, argAry, thd);
  Object keySet = setNew();
  listEach(list, list_keys_callback, keySet, thd);
  return keySet;
}

/*------------------------------------------------------------------*/
Object list_map(Thread* thd, Object args) {
  Object list, fun;
  Object* argAry[] = {&list, &fun};
  primCheckArgs(param_ListAny, args, argAry, thd);
  Object funVal = eval(fun, thd);
  Object q = queueNew();
  while (!listIsEmpty(list)) {
    Object elem = listGetFirst(list);
    Object app = appNew(funVal, listNew(elem, EMPTY_LIST));
    Object val = eval(app, thd);
    queueEnq(q, val);
    list = listGetRest(list);
    if (objGetType(list) != D_List) {
      list = listNew(list, EMPTY_LIST);
    }
  }
  return queueAsList(q);
}

/*------------------------------------------------------------------*/
Object list_reject(Thread* thd, Object args) {
  Object list, fun;
  Object* argAry[] = {&list, &fun};
  primCheckArgs(param_ListAny, args, argAry, thd);
  Object funVal = eval(fun, thd);
  Object q = queueNew();
  while (!listIsEmpty(list)) {
    Object elem = listGetFirst(list);
    Object app = appNew(funVal, listNew(elem, EMPTY_LIST));
    Object val = eval(app, thd);
    if (!objBoolValue(val)) {
      queueEnq(q, elem);
    }
    list = listGetRest(list);
    if (objGetType(list) != D_List) {
      list = listNew(list, EMPTY_LIST);
    }
  }
  return queueAsList(q);
}

/*------------------------------------------------------------------*/
Object list_rest(Thread* thd, Object args) {
  Object list;
  Object* argAry[] = {&list};
  primCheckArgs(param_List, args, argAry, thd);
  return listGetRest(list);
}

/*------------------------------------------------------------------*/
Object list_reverse(Thread* thd, Object args) {
  Object list;
  Object* argAry[] = {&list};
  primCheckArgs(param_List, args, argAry, thd);
  return listReverse(list);
}

/*------------------------------------------------------------------*/
Object list_setFirst(Thread* thd, Object args) {
  Object list, first;
  Object* argAry[] = {&list, &first};
  primCheckArgs(param_ListAny, args, argAry, thd);
  listSetFirst(list, first);
  return list;
}

/*------------------------------------------------------------------*/
Object list_setRest(Thread* thd, Object args) {
  Object list, rest;
  Object* argAry[] = {&list, &rest};
  primCheckArgs(param_ListAny, args, argAry, thd);
  listSetRest(list, rest);
  return list;
}

/*------------------------------------------------------------------*/
Object list_take(Thread* thd, Object args) {
  Object list, nElemsInt;
  Object* argAry[] = {&list, &nElemsInt};
  primCheckArgs(param_ListInt, args, argAry, thd);
  Object q = queueNew();
  Word nElems = intGet(nElemsInt);
  for (int n=0; n<nElems; n++) {
    if (listIsEmpty(list)) {
      break;
    }
    Object elem = listGetFirst(list);
    queueEnq(q, elem);
    list = listGetRest(list);
  }
  return queueAsList(q);
}
