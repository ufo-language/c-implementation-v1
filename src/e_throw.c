#include "delegate.h"
#include "e_throw.h"
#include "object.h"

/*------------------------------------------------------------------*/
Object throwEval(Object thrw, Thread* thd) {
  thd->exception.a = objGetData(thrw, THR_PAYLOAD_OFS);
  longjmp(thd->jumpBuf, 1);
  return nullObj;
}

/*------------------------------------------------------------------*/
void throwFreeVars(Object thrw, Object freeVarSet, Thread* thd) {
  Object payload = {objGetData(thrw, THR_PAYLOAD_OFS)};
  objFreeVars(payload, freeVarSet, thd);
}

/*------------------------------------------------------------------*/
void throwMark(Object thrw) {
  Object payload = {objGetData(thrw, THR_PAYLOAD_OFS)};
  objMark(payload);
}

/*------------------------------------------------------------------*/
Object throwNew(Object expr) {
  Object thrw = objAlloc(E_Throw, THR_OBJ_SIZE);
  objSetData(thrw, THR_PAYLOAD_OFS, expr.a);
  return thrw;
}

/*------------------------------------------------------------------*/
void throwShow(Object thrw, FILE* stream) {
  fputs("throw ", stream);
  Object payload = {objGetData(thrw, THR_PAYLOAD_OFS)};
  objShow(payload, stream);
}
