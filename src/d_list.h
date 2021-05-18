#ifndef D_LIST_H
#define D_LIST_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

#define LST_FIRST_OFS 0
#define LST_REST_OFS 1
#define LST_OBJ_SIZE 2

Object listNew(Object first, Object rest);

Word listCount(Object list);
void listDel(Object list);
void listEach(Object list, void (*fun)(Object data, Object elem, Thread* thd), Object data, Thread* thd);
bool listEquals(Object list, Object other, Thread* thd);
Object listEval(Object list, Thread* thd);
void listFreeVars(Object list, Object freeVarSet, Thread* thd);
Object listGetFirst(Object list);
Object listGetSecond(Object list);
Object listGetThird(Object list);
Object listGetRest(Object list);
Object listGetLastPair(Object list);
bool listIsEmpty(Object list);
Object listLocate(Object list, Object key, Thread* thd); /* expects the list to contain only bindings */
void listMark(Object list);
Object listMatch(Object list, Object other, Object bindingList, Thread* thd);
Object listReverse(Object list);
void listSetFirst(Object list, Object obj);
void listSetRest(Object list, Object obj);
void listShow(Object list, FILE* stream);
void listShowWith(Object list, char* start, char* sep, char* end, FILE* stream);
Object listSplitLast(Object list);

#endif
