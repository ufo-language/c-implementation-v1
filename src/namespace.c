#include "d_prim.h"
#include "d_hash.h"
#include "e_ident.h"
#include "globals.h"
#include "object.h"

Object any_defineAll(Object env);
Object global_defineAll(Object env);
Object hash_defineAll(Object env);
Object io_defineAll(Object env);
Object list_defineAll(Object env);
Object ns_defineAll(Object env);
Object ufo_defineAll(Object env);

void nsAddPrim(Object hash, char* name, PrimFunc func) {
  Object key = identNew(name);
  Object prim = primNew(func);
  hashPut(hash, key, prim);
}

Object nsDefineAll() {
  Object env = EMPTY_LIST;
  env = any_defineAll(env);
  env = global_defineAll(env);
  env = hash_defineAll(env);
  env = io_defineAll(env);
  env = list_defineAll(env);
  env = ns_defineAll(env);
  env = ufo_defineAll(env);
  return env;
}
