#ifndef D_PRIM_H
#define D_PRIM_H

#include <stdio.h>

#include "object.h"

typedef Object (*PrimFunc)(Object args);

#define PRIM_OBJ_SIZE sizeof(PrimFunc)

Object primNew(PrimFunc func);
PrimFunc primGet(Object prim);
void primShow(Object prim, FILE* stream);

#endif
