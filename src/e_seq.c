#include <stdio.h>

#include "d_list.h"
#include "delegate.h"
#include "e_seq.h"
#include "eval.h"
#include "gc.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object seqEval(Object seq, Thread* thd) {
  Object list = {objGetData(seq, SEQ_EXPRS_OFS)};
  Object res = NOTHING;
  while (!listIsEmpty(list)) {
    Object expr = listGetFirst(list);
    res = eval(expr, thd);
    list = listGetRest(list);
  }
  return res;
}

/*------------------------------------------------------------------*/
void seqFreeVars(Object seq, Object freeVarSet) {
  Object list = {objGetData(seq, SEQ_EXPRS_OFS)};
  listFreeVars(list, freeVarSet);
}

/*------------------------------------------------------------------*/
Object seqNew(Object lst) {
  Object seq = objAlloc(E_Seq, 1);
  objSetData(seq, SEQ_EXPRS_OFS, lst.a);
  return seq;
}

/*------------------------------------------------------------------*/
void seqShow(Object seq, FILE* stream) {
  fputs("do ", stream);
  Object list = {objGetData(seq, SEQ_EXPRS_OFS)};
  while (!listIsEmpty(list)) {
    objShow(listGetFirst(list), stream);
    fputc(' ', stream);
    list = listGetRest(list);
  }
  fputs("end", stream);
}
