#include <stdio.h>

#include "d_array.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_string.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object ufo_args(Thread* thd, Object args);

int main_argc;
char** main_argv;

static Object argAry = {0};

/*------------------------------------------------------------------*/
void ufo_defineAll(Object env) {
  char* nsName = "ufo";
  Object ns = hashNew();
  nsAddPrim(ns, "args", ufo_args);
  hashPut(env, identNew(nsName), ns);
}

/*------------------------------------------------------------------*/
Object ufo_args(Thread* thd, Object args) {
  primCheckArgs(EMPTY_LIST, args, thd);
  if (argAry.a != nullObj.a) {
    return argAry;
  }
  argAry = arrayNew(main_argc);
  for (int n=0; n<main_argc; n++) {
    Object argString = stringNew(main_argv[n]);
    arraySet_unsafe(argAry, n, argString);
  }
  return argAry;
}
