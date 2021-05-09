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

Object hash_keys(Thread* thd, Object args);

static Object hash_oneParam;

/*------------------------------------------------------------------*/
Object hash_defineAll(Object env) {
  hash_oneParam = listNew(symbolNew(ObjTypeNames[D_Hash]), EMPTY_LIST);

  Object ns = hashNew();
  nsAddPrim(ns, "keys", hash_keys);
  Object binding = bindingNew(identNew("hash"), ns);
  return listNew(binding, env);
}

/*------------------------------------------------------------------*/
Object hash_keys(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(hash_oneParam, args, thd);
  Object arg = listGetFirst(args);
  return hashKeys(arg);
}
