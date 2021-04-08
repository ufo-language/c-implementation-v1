#ifndef D_EXN_H
#define D_EXN_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

#define EXN_PAYLOAD_OFS 0

Object exnNew(Object payload);

bool exnEqual(Object exn, Object other);
Object exnEval(Object exn, Thread* thd);
Object exnGetPayload(Object exn);
void exnShow(Object exn, FILE* stream);

#endif
