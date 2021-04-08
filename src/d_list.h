#ifndef D_LIST_H
#define D_LIST_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

#define LST_FIRST_OFS 0
#define LST_REST_OFS 1

Object listNew(Object first, Object rest);

Word listCount(Object list);
void listDel(Object list);
bool listEqual(Object list, Object other);
Object listEval(Object list, Thread* thd);
void listFreeVars(Object list, Object freeVarSet);
Object listGetFirst(Object list);
Object listGetRest(Object list);
bool listIsEmpty(Object list);

/* This expects the list to be a list of bindings */
Object listLocate(Object list, Object key);

Object listMatch(Object list, Object other, Object bindingList);
Object listReverse(Object list);
void listSetFirst(Object list, Object obj);
void listSetRest(Object list, Object obj);
void listShow(Object list, FILE* stream);
void listShowWith(Object list, char* start, char* sep, char* end, FILE* stream);

#endif
