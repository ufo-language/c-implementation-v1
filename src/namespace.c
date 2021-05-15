#include "d_prim.h"
#include "d_hash.h"
#include "e_ident.h"
#include "globals.h"
#include "object.h"

void any_defineAll(Object env);
void array_defineAll(Object env);
void global_defineAll(Object env);
void hash_defineAll(Object env);
void io_defineAll(Object env);
void list_defineAll(Object env);
void ns_defineAll(Object env);
void queue_defineAll(Object env);
void seq_defineAll(Object env);
void string_defineAll(Object env);
void stringBuffer_defineAll(Object env);
void tuple_defineAll(Object env);
void type_defineAll(Object env);
void ufo_defineAll(Object env);

void nsAddPrim(Object hash, char* name, PrimFunc func) {
  Object key = identNew(name);
  Object prim = primNew(func);
  hashPut(hash, key, prim);
}

Object nsDefineAll() {
  Object env = hashNew();
  any_defineAll(env);
  array_defineAll(env);
  global_defineAll(env);
  hash_defineAll(env);
  io_defineAll(env);
  list_defineAll(env);
  ns_defineAll(env);
  queue_defineAll(env);
  seq_defineAll(env);
  string_defineAll(env);
  stringBuffer_defineAll(env);
  tuple_defineAll(env);
  type_defineAll(env);
  ufo_defineAll(env);
  return env;
}
