#include "d_prim.h"
#include "d_hash.h"
#include "e_ident.h"
#include "globals.h"
#include "object.h"

Object global_defineAll(Object env);
Object io_defineAll(Object env);

void nsAddPrim(Object hash, char* name, PrimFunc func) {
  Object key = identNew(name);
  Object prim = primNew(func);
  hashPut(hash, key, prim);
}

Object nsDefineAll() {
  Object env = EMPTY_LIST;
  env = global_defineAll(env);
  env = io_defineAll(env);
  return env;
}
