#include <stdio.h>

#include "d_list.h"
#include "defines.h"
#include "delegate.h"
#include "e_app.h"
#include "e_binop.h"
#include "eval.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object binopEval(Object binop, Thread* thd) {
  Object lhs = {objGetData(binop, BINOP_LHS_OFS)};
  Object oper = {objGetData(binop, BINOP_OPER_OFS)};
  Object rhs = {objGetData(binop, BINOP_RHS_OFS)};
  Object args = listNew(lhs, listNew(rhs, EMPTY_LIST));
  Object app = appNew(oper, args);
  return eval(app, thd);
}

/*------------------------------------------------------------------*/
void binopFreeVars(Object binop, Object freeVarSet, Thread* thd) {
  Object lhs = {objGetData(binop, BINOP_LHS_OFS)};
  Object oper = {objGetData(binop, BINOP_OPER_OFS)};
  Object rhs = {objGetData(binop, BINOP_RHS_OFS)};
  objFreeVars(lhs, freeVarSet, thd);
  objFreeVars(oper, freeVarSet, thd);
  objFreeVars(rhs, freeVarSet, thd);
}

/*------------------------------------------------------------------*/
void binopMark(Object binop) {
  Object lhs = {objGetData(binop, BINOP_LHS_OFS)};
  Object oper = {objGetData(binop, BINOP_OPER_OFS)};
  Object rhs = {objGetData(binop, BINOP_RHS_OFS)};
  objMark(lhs);
  objMark(oper);
  objMark(rhs);
}

/*------------------------------------------------------------------*/
Object binopNew(Object lhs, Object oper, Object rhs) {
  Object binop = objAlloc(E_Binop, BINOP_OBJ_SIZE);
  objSetData(binop, BINOP_LHS_OFS, lhs.a);
  objSetData(binop, BINOP_OPER_OFS, oper.a);
  objSetData(binop, BINOP_RHS_OFS, rhs.a);
  return binop;
}

/*------------------------------------------------------------------*/
void binopShow(Object binop, FILE* stream) {
  Object lhs = {objGetData(binop, BINOP_LHS_OFS)};
  Object oper = {objGetData(binop, BINOP_OPER_OFS)};
  Object rhs = {objGetData(binop, BINOP_RHS_OFS)};
  objShow(lhs, stream);
  fputc(' ', stream);
  objShow(oper, stream);
  fputc(' ', stream);
  objShow(rhs, stream);
}
