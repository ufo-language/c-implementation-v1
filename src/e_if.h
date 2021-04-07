#ifndef E_IF_H
#define E_IF_H

#include "object.h"
#include "thread.h"

#define IF_COND_OFS 0
#define IF_CONSEQ_OFS 1
#define IF_ALT_OFS 2

Object ifNew(Object cond, Object conseq, Object alt);

bool ifEqual(Object ifThen, Object other);
Object ifEval(Object ifThen, Thread* thd);
void ifFreeVars(Object ifThen, Object freeVarSet);
void ifMark(Object array);
void ifShow(Object ifThen, FILE* stream);

#endif
