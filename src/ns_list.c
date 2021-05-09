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

Object list_count(Thread* thd, Object args);
Object list_first(Thread* thd, Object args);
Object list_keys(Thread* thd, Object args);
Object list_rest(Thread* thd, Object args);
Object list_reverse(Thread* thd, Object args);

static Object list_oneParam;

/*------------------------------------------------------------------*/
Object list_defineAll(Object env) {
  list_oneParam = listNew(symbolNew(ObjTypeNames[D_List]), EMPTY_LIST);
  Object ns = hashNew();
  nsAddPrim(ns, "count", list_count);
  nsAddPrim(ns, "first", list_first);
  nsAddPrim(ns, "keys", list_keys);
  nsAddPrim(ns, "rest", list_rest);
  nsAddPrim(ns, "reverse", list_reverse);
  Object binding = bindingNew(identNew("list"), ns);
  return listNew(binding, env);
}

/*------------------------------------------------------------------*/
Object list_count(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(list_oneParam, args, thd);
  Object list = listGetFirst(args);
  Word nElems = listCount(list);
  return intNew(nElems);
}

/*------------------------------------------------------------------*/
Object list_first(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(list_oneParam, args, thd);
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
  primCheckArgs(list_oneParam, args, thd);
  Object list = listGetFirst(args);
  Object keySet = setNew();
  listEach(list, list_keys_callback, keySet);
  return keySet;
}

/*------------------------------------------------------------------*/
Object list_rest(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(list_oneParam, args, thd);
  Object list = listGetFirst(args);
  return listGetRest(list);
}

/*------------------------------------------------------------------*/
Object list_reverse(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(list_oneParam, args, thd);
  Object list = listGetFirst(args);
  return listReverse(list);
}
