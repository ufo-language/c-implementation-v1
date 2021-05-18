#ifndef D_EXN_H
#define D_EXN_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

#define EXN_PAYLOAD_OFS 0
#define EXN_OBJ_SIZE 1

Object exnNew(Object payload);

bool exnEquals(Object exn, Object other, Thread* thd);
Object exnEval(Object exn, Thread* thd);
Object exnGetPayload(Object exn);
void exnMark(Object exn);
void exnShow(Object exn, FILE* stream);

#endif
