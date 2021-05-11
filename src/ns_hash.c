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

static Object param_Hash;

/*------------------------------------------------------------------*/
void hash_defineAll(Object env) {
  char* nsName = "hash";
  param_Hash = primBuildTypeList(1, D_Hash);
  Object ns = hashNew();
  nsAddPrim(ns, "keys", hash_keys);
  hashPut(env, identNew(nsName), ns);
}

/*------------------------------------------------------------------*/
Object hash_keys(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_Hash, args, thd);
  Object arg = listGetFirst(args);
  return hashKeys(arg);
}
