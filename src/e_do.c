#include <stdio.h>

#include "d_list.h"
#include "delegate.h"
#include "e_do.h"
#include "eval.h"
#include "globals.h"
#include "object.h"
#include "thread.h"
#include "trampoline.h"

/*------------------------------------------------------------------*/
Object doEval(Object doSeq, Thread* thd) {
  Object exprs = {objGetData(doSeq, DO_EXPRS_OFS)};
  while (!listIsEmpty(exprs)) {
    Object expr = listGetFirst(exprs);
    eval(expr, thd);
    exprs = listGetRest(exprs);
  }
  Object tramp = threadGetTramp(thd);
  Object lastExpr = {objGetData(doSeq, DO_LAST_EXPR_OFS)};
  Object env = threadGetEnv(thd);
  trampSet(tramp, lastExpr, env);
  return tramp;
}

/*------------------------------------------------------------------*/
void doFreeVars(Object doSeq, Object freeVarSet, Thread* thd) {
  Object exprs = {objGetData(doSeq, DO_EXPRS_OFS)};
  listFreeVars(exprs, freeVarSet, thd);
  Object lastExpr = {objGetData(doSeq, DO_LAST_EXPR_OFS)};
  objFreeVars(lastExpr, freeVarSet, thd);
}

/*------------------------------------------------------------------*/
void doMark(Object doSeq) {
  Object exprs = {objGetData(doSeq, DO_EXPRS_OFS)};
  objMark(exprs);
  Object lastExpr = {objGetData(doSeq, DO_LAST_EXPR_OFS)};
  objMark(lastExpr);
}

/*------------------------------------------------------------------*/
Object doNew(Object exprList) {
  if (listIsEmpty(exprList)) {
    exprList = listNew(NOTHING, EMPTY_LIST);
  }
  Object doSeq = objAlloc(E_DoSeq, DO_OBJ_SIZE);
  Object exprParts = listSplitLast(exprList);
  objSetData(doSeq, DO_EXPRS_OFS, listGetFirst(exprParts).a);
  objSetData(doSeq, DO_LAST_EXPR_OFS, listGetRest(exprParts).a);
  return doSeq;
}

/*------------------------------------------------------------------*/
void doShow(Object doSeq, FILE* stream) {
  doShowWith("do ", doSeq, " end", stream);
}

/*------------------------------------------------------------------*/
void doShowWith(char* start, Object doSeq, char* end, FILE* stream) {
  fputs(start, stream);
  Object exprs = {objGetData(doSeq, DO_EXPRS_OFS)};
  while (!listIsEmpty(exprs)) {
    objShow(listGetFirst(exprs), stream);
    fputc(' ', stream);
    exprs = listGetRest(exprs);
  }
  Object lastExpr = {objGetData(doSeq, DO_LAST_EXPR_OFS)};
  objShow(lastExpr, stream);
  fputs(end, stream);
}

