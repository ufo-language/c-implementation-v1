#include <stdio.h>

#include "d_list.h"
#include "delegate.h"
#include "e_seq.h"
#include "eval.h"
#include "globals.h"
#include "object.h"
#include "thread.h"
#include "trampoline.h"

/*------------------------------------------------------------------*/
Object seqEval(Object seq, Thread* thd) {
  Object exprs = {objGetData(seq, SEQ_EXPRS_OFS)};
  while (!listIsEmpty(exprs)) {
    Object expr = listGetFirst(exprs);
    eval(expr, thd);
    exprs = listGetRest(exprs);
  }
  Object tramp = threadGetTramp(thd);
  Object lastExpr = {objGetData(seq, SEQ_LAST_EXPR_OFS)};
  Object env = threadGetEnv(thd);
  trampSet(tramp, lastExpr, env);
  return tramp;
}

/*------------------------------------------------------------------*/
void seqFreeVars(Object seq, Object freeVarSet) {
  Object exprs = {objGetData(seq, SEQ_EXPRS_OFS)};
  listFreeVars(exprs, freeVarSet);
  Object lastExpr = {objGetData(seq, SEQ_LAST_EXPR_OFS)};
  objFreeVars(lastExpr, freeVarSet);
}

/*------------------------------------------------------------------*/
void seqMark(Object seq) {
  Object exprs = {objGetData(seq, SEQ_EXPRS_OFS)};
  objMark(exprs);
  Object lastExpr = {objGetData(seq, SEQ_LAST_EXPR_OFS)};
  objMark(lastExpr);
}

/*------------------------------------------------------------------*/
Object seqNew(Object exprList) {
  if (listIsEmpty(exprList)) {
    exprList = listNew(NOTHING, EMPTY_LIST);
  }
  Object seq = objAlloc(E_Seq, SEQ_OBJ_SIZE);
  Object exprParts = listSplitLast(exprList);
  objSetData(seq, SEQ_EXPRS_OFS, listGetFirst(exprParts).a);
  objSetData(seq, SEQ_LAST_EXPR_OFS, listGetRest(exprParts).a);
  return seq;
}

/*------------------------------------------------------------------*/
void seqShow(Object seq, FILE* stream) {
  seqShowWith("do ", seq, " end", stream);
}

/*------------------------------------------------------------------*/
void seqShowWith(char* start, Object seq, char* end, FILE* stream) {
  fputs(start, stream);
  Object exprs = {objGetData(seq, SEQ_EXPRS_OFS)};
  while (!listIsEmpty(exprs)) {
    objShow(listGetFirst(exprs), stream);
    fputc(' ', stream);
    exprs = listGetRest(exprs);
  }
  Object lastExpr = {objGetData(seq, SEQ_LAST_EXPR_OFS)};
  objShow(lastExpr, stream);
  fputs(end, stream);
}

