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

Object hash_get(Thread* thd, Object args);
Object hash_keys(Thread* thd, Object args);
Object hash_put(Thread* thd, Object args);

static Object param_Hash;
static Object param_HashAny;
static Object param_HashAnyAny;

/*------------------------------------------------------------------*/
void hash_defineAll(Object env) {
  char* nsName = "hash";
  param_Hash = primBuildTypeList(1, D_Hash);
  param_HashAny = primBuildTypeList(1, D_Hash, D_Null);
  param_HashAnyAny = primBuildTypeList(1, D_Hash, D_Null, D_Null);
  Object ns = hashNew();
  nsAddPrim(ns, "get", hash_get);
  nsAddPrim(ns, "keys", hash_keys);
  nsAddPrim(ns, "put", hash_put);
  hashPut(env, identNew(nsName), ns);
}

/*------------------------------------------------------------------*/
Object hash_get(Thread* thd, Object args) {
  primCheckArgs(param_HashAny, args, thd);
  Object hash = listGetFirst(args);
  Object key = listGetSecond(args);
  return hashGet(hash, key, thd);
}

/*------------------------------------------------------------------*/
Object hash_keys(Thread* thd, Object args) {
  primCheckArgs(param_Hash, args, thd);
  Object hash = listGetFirst(args);
  return hashKeys(hash);
}

/*------------------------------------------------------------------*/
Object hash_put(Thread* thd, Object args) {
  primCheckArgs(param_HashAnyAny, args, thd);
  Object hash = listGetFirst(args);
  Object key = listGetSecond(args);
  Object val = listGetThird(args);
  hashPut(hash, key, val);
  return hash;
}
