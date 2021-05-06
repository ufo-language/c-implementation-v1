#include <stdio.h>

#include "defines.h"
#include "delegate.h"
#include "e_if.h"
#include "eval.h"
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
void ifFreeVars(Object ifThen, Object freeVarSet) {
  Object cond = {objGetData(ifThen, IF_COND_OFS)};
  Object conseq = {objGetData(ifThen, IF_CONSEQ_OFS)};
  Object alt = {objGetData(ifThen, IF_ALT_OFS)};
  objFreeVars(cond, freeVarSet);
  objFreeVars(conseq, freeVarSet);
  objFreeVars(alt, freeVarSet);
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
