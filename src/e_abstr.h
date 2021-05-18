#ifndef E_ABSTR_H
#define E_ABSTR_H

#include "object.h"
#include "thread.h"
#include "vmem.h"

/* be sure that these match the offsets in d_closure.h */
#define ABSTR_PARAMS_OFS 0
#define ABSTR_BODY_OFS 1
#define ABSTR_NEXT_OFS 2
#define ABSTR_OBJ_SIZE 3

Object abstrNew(Object params, Object body);

Object abstrEval(Object abstr, Thread* thd);
void abstrFreeVars(Object abstr, Object freeVarSet, Thread* thd);
Object abstrGetBody(Object abstr);
Object abstrGetNext(Object abstr);
Object abstrGetParams(Object abstr);
void abstrMark(Object abstr);
void abstrSetNext(Object abstr, Object nextRule);
void abstrShow(Object abstr, FILE* stream);

#endif
