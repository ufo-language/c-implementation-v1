#include <stdio.h>

#include "d_list.h"
#include "delegate.h"
#include "e_seq.h"
#include "eval.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object seqEval(Object seq, Thread* thd) {
  Object exprs = {objGetData(seq, SEQ_EXPRS_OFS)};
  Object res = NOTHING;
  while (!listIsEmpty(exprs)) {
    Object expr = listGetFirst(exprs);
    res = eval(expr, thd);
    exprs = listGetRest(exprs);
  }
  return res;
}

/*------------------------------------------------------------------*/
void seqFreeVars(Object seq, Object freeVarSet) {
  Object exprs = {objGetData(seq, SEQ_EXPRS_OFS)};
  listFreeVars(exprs, freeVarSet);
}

/*------------------------------------------------------------------*/
void seqMark(Object seq) {
  Object exprs = {objGetData(seq, SEQ_EXPRS_OFS)};
  objMark(exprs);
}

/*------------------------------------------------------------------*/
Object seqNew(Object lst) {
  Object seq = objAlloc(E_Seq, SEQ_OBJ_SIZE);
  objSetData(seq, SEQ_EXPRS_OFS, lst.a);
  return seq;
}

/*------------------------------------------------------------------*/
void seqShow(Object seq, FILE* stream) {
  fputs("do ", stream);
  Object exprs = {objGetData(seq, SEQ_EXPRS_OFS)};
  seqShowExprs(exprs, stream);
  fputs("end", stream);
}

/*------------------------------------------------------------------*/
void seqShowExprs(Object exprList, FILE* stream) {
  while (!listIsEmpty(exprList)) {
    objShow(listGetFirst(exprList), stream);
    fputc(' ', stream);
    exprList = listGetRest(exprList);
  }
}
