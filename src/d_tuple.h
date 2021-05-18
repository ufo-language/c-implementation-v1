#ifndef D_TUPLE_H
#define D_TUPLE_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

#define TUP_NELEMS_OFS 0
#define TUP_HASHCODE_OFS 1
#define TUP_ELEMS_OFS 2
#define TUP_OBJ_SIZE 2

Object tupleN(int nElems, ...);
Object tupleFromArray(Object array);

void tupleDel(Object tuple);

Word tupleCount(Object tuple);
bool tupleEquals(Object tuple, Object other, Thread* thd);
Object tupleEval(Object tuple, Thread* thd);
void tupleFreeVars(Object tuple, Object freeVarSet, Thread* thd);
Object tupleGet(Object tuple, Word index, Thread* thd);
Object tupleGet_unsafe(Object tuple, Word index);
Word tupleHash(Object tuple);
void tupleMark(Object tuple);
Object tupleMatch(Object obj, Object other, Object bindingList, Thread* thd);
void tupleShow(Object tuple, FILE* stream);

#endif
