#ifndef DELEGATE_H
#define DELEGATE_H

#include "object.h"
#include "thread.h"
#include "vmem.h"

bool objBoolValue(Object obj);
void objDisp(Object obj, FILE* stream);
bool objEquals(Object obj1, Object obj2);
Object objEval(Object obj, Thread* thd);
void objFreeVars(Object obj, Object freeVarSet);
Word objHashCode(Object obj);
void objMark(Object obj);
Object objMatch(Object obj, Object other, Object bindingList);
void objShow(Object obj, FILE* stream);

#endif
