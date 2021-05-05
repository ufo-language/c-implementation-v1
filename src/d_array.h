#ifndef D_ARRAY_H
#define D_ARRAY_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

#define ARY_NELEMS_OFS 0
#define ARY_ELEMS_OFS 1
#define ARY_OBJ_SIZE 1

Object arrayNew(Word size);
void arrayDel(Object array);

Word arrayCount(Object array);
void arrayEach(Object array, void (*fun)(Object data, Object elem), Object data);
bool arrayEquals(Object array, Object other);
Object arrayEval(Object array, Thread* thd);
void arrayFill(Object array, Object elem);
void arrayFreeVars(Object array, Object freeVarSet);
Object arrayGet(Object array, Word index, Thread* thd);
Object arrayGet_unsafe(Object array, Word index);
void arrayMark(Object array);
Object arrayMatch(Object array, Object other, Object bindingList);
void arraySet(Object array, Word index, Object obj, Thread* thd);
bool arraySet_unsafe(Object array, Word index, Object obj);
void arrayShow(Object array, FILE* stream);

#endif
