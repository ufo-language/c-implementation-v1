#ifndef E_APP_H
#define E_APP_H

#include "object.h"
#include "thread.h"
#include "vmem.h"

#define APP_ABSTR_OFS 0
#define APP_ARGS_OFS 1
#define APP_OBJ_SIZE 2

Object appNew(Object abstr, Object args);

Object appEval(Object app, Thread* thd);
void appFreeVars(Object app, Object freeVarSet);
void appMark(Object app);
void appShow(Object app, FILE* stream);

#endif
