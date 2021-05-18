#include <stdio.h>

#include "defines.h"
#include "delegate.h"
#include "e_if.h"
#include "eval.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object ifEval(Object ifThen, Thread* thd) {
  Object cond = {objGetData(ifThen, IF_COND_OFS)};
  Object condVal = eval(cond, thd);
  if (objBoolValue(condVal)) {
    Object conseq = {objGetData(ifThen, IF_CONSEQ_OFS)};
    return eval(conseq, thd);
  }
  Object alt = {objGetData(ifThen, IF_ALT_OFS)};
  return eval(alt, thd);
}

/*------------------------------------------------------------------*/
void ifFreeVars(Object ifThen, Object freeVarSet, Thread* thd) {
  Object cond = {objGetData(ifThen, IF_COND_OFS)};
  Object conseq = {objGetData(ifThen, IF_CONSEQ_OFS)};
  Object alt = {objGetData(ifThen, IF_ALT_OFS)};
  objFreeVars(cond, freeVarSet, thd);
  objFreeVars(conseq, freeVarSet, thd);
  objFreeVars(alt, freeVarSet, thd);
}

/*------------------------------------------------------------------*/
void ifMark(Object ifThen) {
  Object cond = {objGetData(ifThen, IF_COND_OFS)};
  Object conseq = {objGetData(ifThen, IF_CONSEQ_OFS)};
  Object alt = {objGetData(ifThen, IF_ALT_OFS)};
  objMark(cond);
  objMark(conseq);
  objMark(alt);
}

/*------------------------------------------------------------------*/
Object ifNew(Object cond, Object conseq, Object alt) {
  Object ifThen = objAlloc(E_If, IF_OBJ_SIZE);
  objSetData(ifThen, IF_COND_OFS, cond.a);
  objSetData(ifThen, IF_CONSEQ_OFS, conseq.a);
  objSetData(ifThen, IF_ALT_OFS, alt.a);
  return ifThen;
}

/*------------------------------------------------------------------*/
void ifShow(Object ifThen, FILE* stream) {
  Object cond = {objGetData(ifThen, IF_COND_OFS)};
  Object conseq = {objGetData(ifThen, IF_CONSEQ_OFS)};
  Object alt = {objGetData(ifThen, IF_ALT_OFS)};
  fprintf(stream, "if ");
  objShow(cond, stream);
  fprintf(stream, " then ");
  objShow(conseq, stream);
  fprintf(stream, " else ");
  objShow(alt, stream);
  fprintf(stream, " end");
}
