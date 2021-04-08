#include "delegate.h"
#include "e_throw.h"
#include "gc.h"
#include "object.h"

Object throwPayload;

/*------------------------------------------------------------------*/
Object throwEval(Object thrw, Thread* thd) {
  throwPayload.a = objGetData(thrw, THR_PAYLOAD_OFS);
  longjmp(thd->jumpBuf, 1);
  return nullObj;
}

/*------------------------------------------------------------------*/
void throwFreeVars(Object thrw, Object freeVarSet) {
  Object payload = {objGetData(thrw, THR_PAYLOAD_OFS)};
  objFreeVars(payload, freeVarSet);
}

/*------------------------------------------------------------------*/
Object throwNew(Object expr) {
  Object thrw = objAlloc(E_Throw, 1);
  objSetData(thrw, THR_PAYLOAD_OFS, expr.a);
  return thrw;
}

/*------------------------------------------------------------------*/
void throwShow(Object thrw, FILE* stream) {
  fputs("throw ", stream);
  Object payload = {objGetData(thrw, THR_PAYLOAD_OFS)};
  objShow(payload, stream);
}
