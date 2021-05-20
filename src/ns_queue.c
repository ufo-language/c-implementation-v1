#include <stdio.h>

#include "d_queue.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object queue_count(Thread* thd, Object args);
Object queue_deq(Thread* thd, Object args);
Object queue_enq(Thread* thd, Object args);

static Object param_Queue;
static Object param_QueueAny;

/*------------------------------------------------------------------*/
void queue_defineAll(Object env, Thread* thd) {
  char* nsName = "queue";
  param_Queue = primBuildTypeList(1, D_Queue);
  param_QueueAny = primBuildTypeList(2, D_Queue, D_Null);
  Object ns = hashNew();
  nsAddPrim(ns, "count", queue_count, thd);
  nsAddPrim(ns, "deq", queue_deq, thd);
  nsAddPrim(ns, "enq", queue_enq, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
Object queue_count(Thread* thd, Object args) {
  Object queue;
  Object* argAry[] = {&queue};
  primCheckArgs(param_Queue, args, argAry, thd);
  Word nElems = queueCount(queue);
  return intNew(nElems);
}

/*------------------------------------------------------------------*/
Object queue_deq(Thread* thd, Object args) {
  Object queue;
  Object* argAry[] = {&queue};
  primCheckArgs(param_Queue, args, argAry, thd);
  Object elem = queueDeq(queue, thd);
  return elem;
}

/*------------------------------------------------------------------*/
Object queue_enq(Thread* thd, Object args) {
  Object queue, elem;
  Object* argAry[] = {&queue, &elem};
  primCheckArgs(param_QueueAny, args, argAry, thd);
  queueEnq(queue, elem);
  return queue;
}
