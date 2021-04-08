#include <stdio.h>

#include "d_exn.h"
#include "defines.h"
#include "delegate.h"
#include "eval.h"
#include "gc.h"
#include "object.h"
#include "thread.h"

Object exnNew(Object payload) {
  Object exn = objAlloc(D_Exn, 1);
  objSetData(exn, 0, payload.a);
  return exn;
}

bool exnEqual(Object exn, Object other) {
  return objEqual(exnGetPayload(exn), exnGetPayload(other));
}

Object exnEval(Object exn, Thread* thd) {
  Object payloadVal = eval(exnGetPayload(exn), thd);
  return exnNew(payloadVal);
}

Object exnGetPayload(Object exn) {
  Object payload = {objGetData(exn, 0)};
  return payload;
}

void exnShow(Object exn, FILE* stream) {
  fputs("Exception{", stream);
  objShow(exnGetPayload(exn), stream);
  fputc('}', stream);
}

