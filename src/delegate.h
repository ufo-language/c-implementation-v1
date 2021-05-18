#ifndef DELEGATE_H
#define DELEGATE_H

#include "object.h"
#include "thread.h"
#include "vmem.h"

bool objBoolValue(Object obj);
Object objCopy(Object obj);
void objDisp(Object obj, FILE* stream);
bool objEquals(Object obj1, Object obj2, Thread* thd);
Object objEval(Object obj, Thread* thd);
void objFreeVars(Object obj, Object freeVarSet, Thread* thd);
Word objHashCode(Object obj);
bool objHasType(Object obj, Object typeSym);
void objMark(Object obj);
Object objMatch(Object obj, Object other, Object bindingList, Thread* thd);
void objShow(Object obj, FILE* stream);

#endif
