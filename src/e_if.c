#include <stdio.h>

#include "defines.h"
#include "delegate.h"
#include "e_if.h"
#include "eval.h"
#include "gc.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
bool ifEqual(Object ifThen, Object other) {
  Object cond1 = {objGetData(ifThen, IF_COND_OFS)};
  Object cond2 = {objGetData(other, IF_COND_OFS)};
  if (!objEquals(cond1, cond2)) {
    return false;
  }
  Object conseq1 = {objGetData(ifThen, IF_CONSEQ_OFS)};
  Object conseq2 = {objGetData(other, IF_CONSEQ_OFS)};
  if (!objEquals(conseq1, conseq2)) {
    return false;
  }
  Object alt1 = {objGetData(ifThen, IF_ALT_OFS)};
  Object alt2 = {objGetData(other, IF_ALT_OFS)};
  return objEquals(alt1, alt2);
}

/*------------------------------------------------------------------*/
Object ifEval(Object ifThen, Thread* thd) {
  Object cond = {objGetData(ifThen, 0)};
  Object condVal = eval(cond, thd);
  if (objBoolValue(condVal)) {
    Object conseq = {objGetData(ifThen, 1)};
    return eval(conseq, thd);
  }
  Object alt = {objGetData(ifThen, 2)};
  return eval(alt, thd);
}

/*------------------------------------------------------------------*/
void ifFreeVars(Object ifThen, Object freeVarSet) {
  Object cond = {objGetData(ifThen, 0)};
  Object conseq = {objGetData(ifThen, 1)};
  Object alt = {objGetData(ifThen, 2)};
  objFreeVars(cond, freeVarSet);
  objFreeVars(conseq, freeVarSet);
  objFreeVars(alt, freeVarSet);
}

/*------------------------------------------------------------------*/
void ifMark(Object ifThen) {
  Object cond = {objGetData(ifThen, 0)};
  Object conseq = {objGetData(ifThen, 1)};
  Object alt = {objGetData(ifThen, 2)};
  objMark(cond);
  objMark(conseq);
  objMark(alt);
}

/*------------------------------------------------------------------*/
Object ifNew(Object cond, Object conseq, Object alt) {
  Object ifThen = objAlloc(E_If, IF_OBJ_SIZE);
  objSetData(ifThen, 0, cond.a);
  objSetData(ifThen, 1, conseq.a);
  objSetData(ifThen, 2, alt.a);
  return ifThen;
}

/*------------------------------------------------------------------*/
void ifShow(Object ifThen, FILE* stream) {
  Object cond = {objGetData(ifThen, 0)};
  Object conseq = {objGetData(ifThen, 1)};
  Object alt = {objGetData(ifThen, 2)};
  fprintf(stream, "if ");
  objShow(cond, stream);
  fprintf(stream, " then ");
  objShow(conseq, stream);
  fprintf(stream, " else ");
  objShow(alt, stream);
  fprintf(stream, " end");
}
