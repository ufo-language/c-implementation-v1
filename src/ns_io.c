#include <stdio.h>

#include "d_binding.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_prim.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object io_print(Thread* thd, Object args);

Object io_defineAll(Object env) {
  Object ns = hashNew();
  nsAddPrim(ns, "print", io_print);
  Object binding = bindingNew(identNew("io"), ns);
  return listNew(binding, env);
}

static void printCallback(Object data, Object elem) {
  (void)data;
  objDisp(elem, stdout);
}

Object io_print(Thread* thd, Object args) {
  (void)thd;
  listEach(args, printCallback, NOTHING);
  return NOTHING;
}
