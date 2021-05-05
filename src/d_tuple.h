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

Object tuple1(Object elem0);
Object tuple2(Object elem0, Object elem1);
Object tuple3(Object elem0, Object elem1, Object elem2);
Object tuple4(Object elem0, Object elem1, Object elem2, Object elem3);
Object tupleFromArray(Object array);

void tupleDel(Object tuple);

Word tupleCount(Object tuple);
bool tupleEquals(Object tuple, Object other);
Object tupleEval(Object tuple, Thread* thd);
void tupleFreeVars(Object tuple, Object freeVarSet);
Object tupleGet(Object tuple, Word index, Thread* thd);
Object tupleGet_unsafe(Object tuple, Word index);
Word tupleHash(Object tuple);
void tupleMark(Object tuple);
Object tupleMatch(Object obj, Object other, Object bindingList);
void tupleShow(Object tuple, FILE* stream);

#endif
