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
void ns_defineAll(Object env) {
  char* nsName = "ns";
  Object ns = hashNew();
  nsAddPrim(ns, "global", ns_global);
  hashPut(env, identNew(nsName), ns);
}

/*------------------------------------------------------------------*/
Object ns_global(Thread* thd, Object args) {
  primCheckArgs2(EMPTY_LIST, args, NULL, thd);
  return GLOBALS;
}
