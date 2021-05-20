#ifndef D_PRIM_H
#define D_PRIM_H

#include "object.h"
#include "thread.h"

typedef Object (*PrimFunc)(Thread* thd, Object args);

#define PRIM_OBJ_SIZE (sizeof(PrimFunc) / sizeof(Word))

Object primNew(PrimFunc func);
Object primMacroNew(PrimFunc func);

Object primApply(Object closure, Object argList, Thread* thd);
Object primBuildTypeList(int nArgs, ...);

void primArgError(Word n, Object paramTypes, Object args, Thread* thd);
void primCheckArgs(Object paramTypes, Object args, Object** argVars, Thread* thd);
int primCheckArgs_aux(Object paramTypes, Object args, Object** argVars);

PrimFunc primGet(Object prim);
void primShow(Object prim, FILE* stream);

#endif
