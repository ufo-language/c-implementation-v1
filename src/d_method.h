#ifndef D_METHOD_H
#define D_METHOD_H

#include "object.h"
#include "thread.h"

#define METH_RECEIVER_OFS 0
#define METH_FUNC_OFS 1
#define METH_OBJ_SIZE 2

Object methodNew(Object receiver, Object func);

Object methodApply(Object method, Object argList, Thread* thd);
void methodMark(Object method);
void methodShow(Object method, FILE* stream);

#endif
