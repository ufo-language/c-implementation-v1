#include <stdio.h>

#include "d_exn.h"
#include "defines.h"
#include "delegate.h"
#include "eval.h"
#include "gc.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object exnNew(Object payload) {
  Object exn = objAlloc(D_Exn, EXN_OBJ_SIZE);
  objSetData(exn, 0, payload.a);
  return exn;
}

/*------------------------------------------------------------------*/
bool exnEquals(Object exn, Object other) {
  return objEquals(exnGetPayload(exn), exnGetPayload(other));
}

/*------------------------------------------------------------------*/
Object exnEval(Object exn, Thread* thd) {
  Object payloadVal = eval(exnGetPayload(exn), thd);
  return exnNew(payloadVal);
}

/*------------------------------------------------------------------*/
Object exnGetPayload(Object exn) {
  Object payload = {objGetData(exn, EXN_PAYLOAD_OFS)};
  return payload;
}

/*------------------------------------------------------------------*/
void exnMark(Object exn) {
  Object payload = {objGetData(exn, EXN_PAYLOAD_OFS)};
  objMark(payload);
}

/*------------------------------------------------------------------*/
void exnShow(Object exn, FILE* stream) {
  fputs("Exception{", stream);
  objShow(exnGetPayload(exn), stream);
  fputc('}', stream);
}

