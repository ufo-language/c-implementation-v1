#include "d_prim.h"
#include "d_hash.h"
#include "e_ident.h"
#include "globals.h"
#include "object.h"

void any_defineAll(Object env, Thread* thd);
void array_defineAll(Object env, Thread* thd);
void global_defineAll(Object env, Thread* thd);
void hash_defineAll(Object env, Thread* thd);
void io_defineAll(Object env, Thread* thd);
void list_defineAll(Object env, Thread* thd);
void ns_defineAll(Object env, Thread* thd);
void queue_defineAll(Object env, Thread* thd);
void seq_defineAll(Object env, Thread* thd);
void string_defineAll(Object env, Thread* thd);
void stringBuffer_defineAll(Object env, Thread* thd);
void tuple_defineAll(Object env, Thread* thd);
void type_defineAll(Object env, Thread* thd);
void ufo_defineAll(Object env, Thread* thd);

void nsAddPrim(Object hash, char* name, PrimFunc func, Thread* thd) {
  Object key = identNew(name);
  Object prim = primNew(func);
  hashPut(hash, key, prim, thd);
}

Object nsDefineAll(Thread* thd) {
  Object env = hashNew();
  any_defineAll(env, thd);
  array_defineAll(env, thd);
  global_defineAll(env, thd);
  hash_defineAll(env, thd);
  io_defineAll(env, thd);
  list_defineAll(env, thd);
  ns_defineAll(env, thd);
  queue_defineAll(env, thd);
  seq_defineAll(env, thd);
  string_defineAll(env, thd);
  stringBuffer_defineAll(env, thd);
  tuple_defineAll(env, thd);
  type_defineAll(env, thd);
  ufo_defineAll(env, thd);
  return env;
}
