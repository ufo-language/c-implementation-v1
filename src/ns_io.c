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

Object io_disp(Thread* thd, Object args);
Object io_displn(Thread* thd, Object args);
Object io_nl(Thread* thd, Object args);
Object io_show(Thread* thd, Object args);
Object io_showln(Thread* thd, Object args);

/*------------------------------------------------------------------*/
void io_defineAll(Object env, Thread* thd) {
  char* nsName = "io";
  Object ns = hashNew();
  nsAddPrim(ns, "disp", io_disp, thd);
  nsAddPrim(ns, "displn", io_displn, thd);
  nsAddPrim(ns, "nl", io_nl, thd);
  nsAddPrim(ns, "show", io_show, thd);
  nsAddPrim(ns, "showln", io_showln, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
static void dispCallback(Object data, Object elem, Thread* thd) {
  (void)data;
  (void)thd;
  objDisp(elem, stdout);
}

/*------------------------------------------------------------------*/
static void showCallback(Object data, Object elem, Thread* thd) {
  (void)data;
  (void)thd;
  objShow(elem, stdout);
}

/*------------------------------------------------------------------*/
Object io_disp(Thread* thd, Object args) {
  listEach(args, dispCallback, NOTHING, thd);
  return NOTHING;
}

/*------------------------------------------------------------------*/
Object io_displn(Thread* thd, Object args) {
  listEach(args, dispCallback, NOTHING, thd);
  printf("\n");
  return NOTHING;
}

/*------------------------------------------------------------------*/
Object io_nl(Thread* thd, Object args) {
  primCheckArgs2(EMPTY_LIST, args, NULL, thd);
  printf("\n");
  return NOTHING;
}

/*------------------------------------------------------------------*/
Object io_show(Thread* thd, Object args) {
  listEach(args, showCallback, NOTHING, thd);
  return NOTHING;
}

/*------------------------------------------------------------------*/
Object io_showln(Thread* thd, Object args) {
  listEach(args, showCallback, NOTHING, thd);
  printf("\n");
  return NOTHING;
}
