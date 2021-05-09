#ifndef D_PRIM_H
#define D_PRIM_H

#include "object.h"
#include "thread.h"

typedef Object (*PrimFunc)(Thread* thd, Object args);

#define PRIM_OBJ_SIZE (sizeof(PrimFunc) / sizeof(Word))

Object primNew(PrimFunc func);
Object primMacroNew(PrimFunc func);

Object primApply(Object closure, Object argList, Thread* thd);
void primCheckArgs(Object params, Object args, Thread* thd);
PrimFunc primGet(Object prim);
void primShow(Object prim, FILE* stream);

#endif
