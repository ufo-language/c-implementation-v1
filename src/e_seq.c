#include <stdio.h>

#include "d_list.h"
#include "delegate.h"
#include "e_seq.h"
#include "eval.h"
#include "gc.h"
#include "globals.h"
#include "object.h"
#include "thread.h"
#include "trampoline.h"

/*------------------------------------------------------------------*/
Object seqEval(Object seq, Thread* thd) {
  Object list = {objGetData(seq, SEQ_EXPRS_OFS)};
  while (!listIsEmpty(list)) {
    Object expr = listGetFirst(list);
    eval(expr, thd);
    list = listGetRest(list);
  }
  Object last = {objGetData(seq, SEQ_LAST_EXPR_OFS)};
  Object tramp = trampNew(last, threadGetEnv(thd));
  return tramp;
}

/*------------------------------------------------------------------*/
void seqFreeVars(Object seq, Object freeVarSet) {
  Object list = {objGetData(seq, SEQ_EXPRS_OFS)};
  listFreeVars(list, freeVarSet);
  Object last = {objGetData(seq, SEQ_LAST_EXPR_OFS)};
  objFreeVars(last, freeVarSet);
}

/*------------------------------------------------------------------*/
Object seqNew(Object lst) {
  /* split the list into all-but-last and last */
  Object pair = listSplitLast(lst);
  Object allButLast = listGetFirst(pair);
  Object last = listGetRest(pair);
  Object seq = objAlloc(E_Seq, 2);
  objSetData(seq, SEQ_EXPRS_OFS, allButLast.a);
  objSetData(seq, SEQ_LAST_EXPR_OFS, last.a);
  return seq;
}

/*------------------------------------------------------------------*/
void seqShow(Object seq, FILE* stream) {
  fputs("do ", stream);
  seqShowExprs(seq, stream);
  fputs("end", stream);
}

/*------------------------------------------------------------------*/
void seqShowExprs(Object seq, FILE* stream) {
  Object exprList = {objGetData(seq, SEQ_EXPRS_OFS)};
  while (!listIsEmpty(exprList)) {
    objShow(listGetFirst(exprList), stream);
    fputc(' ', stream);
    exprList = listGetRest(exprList);
  }
  Object last = {objGetData(seq, SEQ_LAST_EXPR_OFS)};
  objShow(last, stream);
  fputc(' ', stream);
}
