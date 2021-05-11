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
void list_defineAll(Object env) {
  char* nsName = "list";
  param_List = primBuildTypeList(1, D_List);
  param_ListAny = primBuildTypeList(2, D_List, D_Null);
  param_ListInt = primBuildTypeList(2, D_List, D_Int);
  Object ns = hashNew();
  nsAddPrim(ns, "accept", list_accept);
  nsAddPrim(ns, "drop", list_drop);
  nsAddPrim(ns, "count", list_count);
  nsAddPrim(ns, "first", list_first);
  nsAddPrim(ns, "keys", list_keys);
  nsAddPrim(ns, "map", list_map);
  nsAddPrim(ns, "reject", list_reject);
  nsAddPrim(ns, "rest", list_rest);
  nsAddPrim(ns, "setFirst", list_setFirst);
  nsAddPrim(ns, "setRest", list_setRest);
  nsAddPrim(ns, "reverse", list_reverse);
  nsAddPrim(ns, "take", list_take);
  hashPut(env, identNew(nsName), ns);
}

/*------------------------------------------------------------------*/
Object list_accept(Thread* thd, Object args) {
  primCheckArgs(param_ListAny, args, thd);
  Object list = listGetFirst(args);
  Object fun = listGetSecond(args);
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
  (void)thd;
  primCheckArgs(param_List, args, thd);
  Object list = listGetFirst(args);
  Word nElems = listCount(list);
  return intNew(nElems);
}

/*------------------------------------------------------------------*/
Object list_drop(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_ListInt, args, thd);
  Object list = listGetFirst(args);
  Object nElemsInt = listGetSecond(args);
  Word nElems = intGet(nElemsInt);
  for (int n=0; n<nElems; n++) {
    list = listGetRest(list);
  }
  return list;
}

/*------------------------------------------------------------------*/
Object list_first(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_List, args, thd);
  Object list = listGetFirst(args);
  return listGetFirst(list);
}

/*------------------------------------------------------------------*/
static void list_keys_callback(Object keySet, Object elem) {
  if (D_Binding == objGetType(elem)) {
    setAddElem(keySet, bindingGetLhs(elem));
  }
}

/*------------------------------------------------------------------*/
Object list_keys(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_List, args, thd);
  Object list = listGetFirst(args);
  Object keySet = setNew();
  listEach(list, list_keys_callback, keySet);
  return keySet;
}

/*------------------------------------------------------------------*/
Object list_map(Thread* thd, Object args) {
  primCheckArgs(param_ListAny, args, thd);
  Object list = listGetFirst(args);
  Object fun = listGetSecond(args);
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
  primCheckArgs(param_ListAny, args, thd);
  Object list = listGetFirst(args);
  Object fun = listGetSecond(args);
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
  (void)thd;
  primCheckArgs(param_List, args, thd);
  Object list = listGetFirst(args);
  return listGetRest(list);
}

/*------------------------------------------------------------------*/
Object list_reverse(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_List, args, thd);
  Object list = listGetFirst(args);
  return listReverse(list);
}

/*------------------------------------------------------------------*/
Object list_setFirst(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_ListAny, args, thd);
  Object list = listGetFirst(args);
  Object first = listGetSecond(args);
  listSetFirst(list, first);
  return list;
}

/*------------------------------------------------------------------*/
Object list_setRest(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_ListAny, args, thd);
  Object list = listGetFirst(args);
  Object rest = listGetSecond(args);
  listSetRest(list, rest);
  return list;
}

/*------------------------------------------------------------------*/
Object list_take(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_ListInt, args, thd);
  Object list = listGetFirst(args);
  Object q = queueNew();
  Object nElemsInt = listGetSecond(args);
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
