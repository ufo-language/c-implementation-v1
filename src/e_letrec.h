#ifndef E_LETREC_H
#define E_LETREC_H

#include "object.h"
#include "thread.h"

#define LETREC_BINDINGS_OFS 0
#define LETREC_OBJ_SIZE 1

Object letRecNew(Object bindings);

Object letRecEval(Object letRec, Thread* thd);
void letRecFreeVars(Object letRec, Object freeVarSet, Thread* thd);
void letRecMark(Object letRec);
void letRecShow(Object letRec, FILE* stream);

#endif
