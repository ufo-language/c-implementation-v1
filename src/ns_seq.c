#include <stdio.h>

#include "d_binding.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_seq.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object seq_new(Thread* thd, Object args);

static Object param_AnyAny;

/*------------------------------------------------------------------*/
Object seq_defineAll(Object env) {
  param_AnyAny = primBuildTypeList(2, D_Null, D_Null);
  Object ns = hashNew();
  nsAddPrim(ns, "new", seq_new);
  Object binding = bindingNew(identNew("seq"), ns);
  return listNew(binding, env);
}

/*------------------------------------------------------------------*/
Object seq_new(Thread* thd, Object args) {
  (void)thd;
  primCheckArgs(param_AnyAny, args, thd);
  Object from = listGetFirst(args);
  Object to = listGetSecond(args);
  Object seq = seqNew(from, to, intNew(1));
  return seq;
}
