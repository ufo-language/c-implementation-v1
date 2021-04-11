#ifndef D_QUEUE_H
#define D_QUEUE_H

#include "object.h"
#include "thread.h"

#define Q_NELEMS_OFS 0
#define Q_HEAD_OFS 1
#define Q_TAIL_OFS 2

Object queueNew();

Object queueAsList(Object q);
Word queueCount(Object q);
void queueEnq(Object q, Object elem);
Word queueCount(Object q);
bool queueEqual(Object q, Object other);
Object queueEval(Object q, Thread* thd);
void queueFreeVars(Object q, Object freeVarSet);
Object queueDeq(Object q);
void queueShow(Object q, FILE* stream);

#endif

