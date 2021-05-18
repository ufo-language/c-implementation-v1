#ifndef D_SET_H
#define D_SET_H

#include "object.h"
#include "thread.h"

#define SET_NELEMS_OFS 0
#define SET_LOADINGFACTOR_OFS 1
#define SET_BUCKETS_OFS 2
#define SET_OBJ_SIZE 3

Object setNew(void);

void setAddElem(Object set, Object elem, Thread* thd);
Word setCount(Object set);
bool setEquals(Object set, Object other, Thread* thd);
Object setEval(Object set, Thread* thd);
void setFreeVars(Object set, Object freeVarSet, Thread* thd);
bool setHas(Object set, Object elem, Thread* thd);
void setMark(Object set);
bool setRemoveElem(Object set, Object elem, Thread* thd);
void setRemoveSet(Object set, Object otherSet, Thread* thd);
void setShow(Object set, FILE* stream);
Object setToArray(Object set);
void setUnion(Object set, Object otherSet, Thread* thd);

#endif

