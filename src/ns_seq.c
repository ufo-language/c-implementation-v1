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
void seq_defineAll(Object env, Thread* thd) {
  char* nsName = "seq";
  param_AnyAny = primBuildTypeList(2, D_Null, D_Null);
  Object ns = hashNew();
  nsAddPrim(ns, "new", seq_new, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
Object seq_new(Thread* thd, Object args) {
  Object from, to;
  Object* argAry[] = {&from, &to};
  primCheckArgs(param_AnyAny, args, argAry, thd);
  Object seq = seqNew(from, to, intNew(1), thd);
  return seq;
}
