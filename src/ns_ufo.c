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

/*------------------------------------------------------------------*/
void ufo_defineAll(Object env, Thread* thd) {
  char* nsName = "ufo";
  Object ns = hashNew();
  nsAddPrim(ns, "args", ufo_args, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
Object ufo_args(Thread* thd, Object args) {
  primCheckArgs(EMPTY_LIST, args, NULL, thd);
  Object cmdLineArgs = arrayNew(main_argc);
  for (int n=0; n<main_argc; n++) {
    Object argString = stringNew(main_argv[n]);
    arraySet_unsafe(cmdLineArgs, n, argString);
  }
  return cmdLineArgs;
}
