#include <stdio.h>

#include "d_binding.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_prim.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "object.h"

static void _addPrim(Object hash, char* name, PrimFunc func) {
  Object key = identNew(name);
  Object prim = primNew(func);
  hashPut(hash, key, prim);
}

Object io_print(Object args);

Object io_defineAll(Object env) {
  Object ns = hashNew();
  _addPrim(ns, "print", io_print);
  Object binding = bindingNew(identNew("io"), ns);
  return listNew(binding, env);
}

Object io_print(Object args) {
  printf("io_print args = "); objShow(args, stdout); printf("\n");
  return NOTHING;
}
