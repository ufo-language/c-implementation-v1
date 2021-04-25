#include <stdio.h>

#include "d_binding.h"
#include "d_list.h"
#include "delegate.h"
#include "eval.h"
#include "gc.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Word listCount(Object list) {
  Word n = 0;
  while (!listIsEmpty(list)) {
    n++;
    list = listGetRest(list);
    if (D_List != objGetType(list)) {
      n++;
      break;
    }
  }
  return n;
}

/*------------------------------------------------------------------*/
Object listCreateEmpty() {
  Object list = listNew(nullObj, nullObj);
  objSetData(list, LST_FIRST_OFS, NOTHING.a);
  objSetData(list, LST_REST_OFS, list.a);
  return list;
}

/*------------------------------------------------------------------*/
void listEach(Object list, void (*fun)(Object data, Object elem), Object data) {
  while (!listIsEmpty(list)) {
    Object elem = listGetFirst(list);
    fun(data, elem);
    list = listGetRest(list);
    if (objGetType(list) != D_List) {
      list = listNew(list, EMPTY_LIST);
    }
  }
}

/*------------------------------------------------------------------*/
bool listEquals(Object list, Object other) {
  if (listIsEmpty(list)) {
    return listIsEmpty(other);
  }
  if (listIsEmpty(other)) {
    return false;
  }
  Object first1 = listGetFirst(list);
  Object first2 = listGetFirst(other);
  if (!objEquals(first1, first2)) {
    return false;
  }
  Object rest1 = listGetRest(list);
  Object rest2 = listGetRest(other);
  return objEquals(rest1, rest2);
}

/*------------------------------------------------------------------*/
Object listEval(Object list, Thread* thd) {
  if (listIsEmpty(list)) {
    return EMPTY_LIST;
  }
  Object newFirst = eval(listGetFirst(list), thd);
  Object newRest = eval(listGetRest(list), thd);
  return listNew(newFirst, newRest);
}

/*------------------------------------------------------------------*/
void listFreeVars(Object list, Object freeVarSet) {
  if (listIsEmpty(list)) {
    return;
  }
  objFreeVars(listGetFirst(list), freeVarSet);
  objFreeVars(listGetRest(list), freeVarSet);
}

/*------------------------------------------------------------------*/
Object listGetFirst(Object list) {
  Object obj = {objGetData(list, LST_FIRST_OFS)};
  return obj;
}

/*------------------------------------------------------------------*/
Object listGetSecond(Object list) {
  return listGetFirst(listGetRest(list));
}

/*------------------------------------------------------------------*/
Object listGetThird(Object list) {
  return listGetFirst(listGetRest(listGetRest(list)));
}

/*------------------------------------------------------------------*/
Object listGetRest(Object list) {
  Object obj = {objGetData(list, LST_REST_OFS)};
  return obj;
}

/*------------------------------------------------------------------*/
Object listGetLastPair(Object list) {
  if (listIsEmpty(list)) {
    return nullObj;
  }
  while (true) {
    Object rest = listGetRest(list);
    if (listIsEmpty(rest)) {
      return list;
    }
    list = rest;
  }
}

/*------------------------------------------------------------------*/
bool listIsEmpty(Object list) {
  return list.a == EMPTY_LIST.a;
}

/*------------------------------------------------------------------*/
Object listLocate(Object list, Object key) {
  while (!listIsEmpty(list)) {
    Object binding = listGetFirst(list);
    if (D_Binding == objGetType(binding)) {
      if (objEquals(key, bindingGetLhs(binding))) {
        return binding;
      }
    }
    else {
      /* exit if the list contains something other than a binding */
      break;
    }
    list = listGetRest(list);
  }
  return nullObj;
}

/*------------------------------------------------------------------*/
Object listMatch(Object list, Object other, Object bindingList) {
  if (listIsEmpty(list)) {
    return listIsEmpty(other) ? bindingList : nullObj;
  }
  Object first1 = listGetFirst(list);
  Object first2 = listGetFirst(other);
  bindingList = objMatch(first1, first2, bindingList);
  if (bindingList.a == nullObj.a) {
    return nullObj;
  }
  Object rest1 = listGetRest(list);
  Object rest2 = listGetRest(other);
  return objMatch(rest1, rest2, bindingList);
}

/*------------------------------------------------------------------*/
Object listNew(Object first, Object rest) {
  Object list = objAlloc(D_List, 2);
  objSetData(list, LST_FIRST_OFS, first.a);
  objSetData(list, LST_REST_OFS, rest.a);
  return list;
}

/*------------------------------------------------------------------*/
Object listReverse(Object list) {
  Object rev = EMPTY_LIST;
  while (!listIsEmpty(list)) {
    rev = listNew(listGetFirst(list), rev);
    Object rest = listGetRest(list);
    if (objGetType(rest) == D_List) {
      list = rest;
    }
    else {
      rev = listNew(rest, rev);
      break;
    }
  }
  return rev;
}

/*------------------------------------------------------------------*/
void listSetFirst(Object list, Object obj) {
  if (!listIsEmpty(list)) {
    objSetData(list, LST_FIRST_OFS, obj.a);
  }
}

/*------------------------------------------------------------------*/
void listSetRest(Object list, Object obj) {
  if (!listIsEmpty(list)) {
    objSetData(list, LST_REST_OFS, obj.a);
  }
}

/*------------------------------------------------------------------*/
void listShow(Object list, FILE* stream) {
  listShowWith(list, "[", ", ", "]", stream);
}

/*------------------------------------------------------------------*/
void listShowWith(Object list, char* start, char* sep, char* end, FILE* stream) {
  bool firstIter = true;
  fputs(start, stream);
  while (!listIsEmpty(list)) {
    if (firstIter) {
      firstIter = false;
    }
    else {
      fputs(sep, stream);
    }
    objShow(listGetFirst(list), stream);
    Object rest = listGetRest(list);
    if (objGetType(rest) == D_List) {
      list = rest;
    }
    else {
      fputs(" | ", stream);
      objShow(rest, stream);
      break;
    }
  }
  fputs(end, stream);
}
