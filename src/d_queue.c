#include "d_array.h"
#include "d_int.h"
#include "d_list.h"
#include "d_queue.h"
#include "delegate.h"
#include "eval.h"
#include "globals.h"
#include "object.h"

/*------------------------------------------------------------------*/
Object queueAsList(Object q) {
  Object list = {objGetData(q, Q_HEAD_OFS)};
  return list;
}

/*------------------------------------------------------------------*/
Word queueCount(Object q) {
  return objGetData(q, Q_NELEMS_OFS);
}

/*------------------------------------------------------------------*/
Object queueDeq(Object q, Thread* thd) {
  Object elem = queueDeq_unsafe(q);
  if (elem.a == nullObj.a) {
    threadThrowException(thd, "Error", "Queue empty", q);
  }
  return elem;
}

/*------------------------------------------------------------------*/
Object queueDeq_unsafe(Object q) {
  Word nElems = objGetData(q, Q_NELEMS_OFS);
  if (nElems == 0) {
  }
  Object head = {objGetData(q, Q_HEAD_OFS)};
  Object elem = listGetFirst(head);
  Object rest = listGetRest(head);
  objSetData(q, Q_HEAD_OFS, rest.a);
  objDecData(q, Q_NELEMS_OFS);
  if (listIsEmpty(rest)) {
    objSetData(q, Q_TAIL_OFS, rest.a);
  }
  return elem;
}

/*------------------------------------------------------------------*/
bool queueEquals(Object q, Object other) {
  Object elems1 = {objGetData(q, Q_HEAD_OFS)};
  Object elems2 = {objGetData(other, Q_HEAD_OFS)};
  return listEquals(elems1, elems2);
}

/*------------------------------------------------------------------*/
void queueEnq(Object q, Object elem) {
  Word nElems = objGetData(q, Q_NELEMS_OFS);
  Object list = listNew(elem, EMPTY_LIST);
  if (nElems == 0) {
    objSetData(q, Q_HEAD_OFS, list.a);
  }
  else {
    Object tail = {objGetData(q, Q_TAIL_OFS)};
    listSetRest(tail, list);
  }
  objSetData(q, Q_TAIL_OFS, list.a);
  objIncData(q, Q_NELEMS_OFS);
}

/*------------------------------------------------------------------*/
Object queueEval(Object q, Thread* thd) {
  Object q1 = queueNew();
  Object elems = {objGetData(q, Q_HEAD_OFS)};
  while (!listIsEmpty(elems)) {
    Object elem = listGetFirst(elems);
    Object elem1 = objEval(elem, thd);
    queueEnq(q1, elem1);
    elems = listGetRest(elems);
  }
  return q1;
}

/*------------------------------------------------------------------*/
void queueFreeVars(Object q, Object freeVarSet) {
  Object elems = {objGetData(q, Q_HEAD_OFS)};
  while (!listIsEmpty(elems)) {
    Object elem = listGetFirst(elems);
    objFreeVars(elem, freeVarSet);
    elems = listGetRest(elems);
  }
}

/*------------------------------------------------------------------*/
void queueMark(Object q) {
  Object elems = {objGetData(q, Q_HEAD_OFS)};
  objMark(elems);
}

/*------------------------------------------------------------------*/
Object queueNew(void) {
  Object q = objAlloc(D_Queue, Q_OBJ_SIZE);
  objSetData(q, Q_NELEMS_OFS, 0);
  objSetData(q, Q_HEAD_OFS, EMPTY_LIST.a);
  objSetData(q, Q_TAIL_OFS, EMPTY_LIST.a);
  return q;
}

/*------------------------------------------------------------------*/
void queueShow(Object q, FILE* stream) {
  Object elems = {objGetData(q, Q_HEAD_OFS)};
  listShowWith(elems, "~[", ", ", "]", stream);
}
