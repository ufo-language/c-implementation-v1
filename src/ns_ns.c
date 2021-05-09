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

Object ns_global(Thread* thd, Object args);

/*------------------------------------------------------------------*/
Object ns_defineAll(Object env) {
  Object ns = hashNew();
  nsAddPrim(ns, "global", ns_global);
  Object binding = bindingNew(identNew("ns"), ns);
  return listNew(binding, env);
}

/*------------------------------------------------------------------*/
Object ns_global(Thread* thd, Object args) {
  (void)thd;
  (void)args;
  primCheckArgs(EMPTY_LIST, args, thd);
  return threadGetEnv(thd);
}
