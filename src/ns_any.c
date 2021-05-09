#include <stdio.h>

#include "d_binding.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object any_match(Thread* thd, Object args);
Object any_type(Thread* thd, Object args);

static Object any_match_paramTypes;
static Object any_type_paramTypes;

Object any_defineAll(Object env) {
  Object ns = hashNew();

  nsAddPrim(ns, "match", any_match);
  any_match_paramTypes = listNew(symbolNew("Any"), listNew(symbolNew("Any"), EMPTY_LIST));
  nsAddPrim(ns, "type", any_type);
  any_type_paramTypes = listNew(identNew("Any"), EMPTY_LIST);

  Object binding = bindingNew(identNew("any"), ns);
  return listNew(binding, env);
}

Object any_match(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(any_match_paramTypes, args, thd);
  return objMatch(listGetFirst(args), listGetSecond(args), EMPTY_LIST);
}

Object any_type(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(any_type_paramTypes, args, thd);
  Object arg = listGetFirst(args);
  ObjType objType = objGetType(arg);
  char* typeName = ObjTypeNames[objType];
  Object typeSym = symbolNew(typeName);
  return typeSym;
}
