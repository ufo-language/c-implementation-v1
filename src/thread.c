#include <stdlib.h>

#include "d_binding.h"
#include "d_list.h"
#include "delegate.h"
#include "eval.h"
#include "gc.h"
#include "globals.h"
#include "thread.h"

Thread* runningThreads = 0;
Thread* threadPool = 0;

void threadDelete(Thread* thd) {
  if (thd->prev) {
    thd->prev->next = thd->next;
  }
  else {
    runningThreads = thd->next;
  }
  if (thd->next) {
    thd->next->prev = thd->prev;
  }
  thd->next = threadPool;
  threadPool = thd;
}

void threadEnvBind(Thread* thd, Object var, Object val) {
  Object env = threadGetEnv(thd);
  Object binding = bindingNew(var, val);
  env = listNew(binding, env);
  threadSetEnv(thd, env);
}

Object threadEval(Thread* thd, Object expr, Object bindings) {
  Object savedEnv = thd->env;
  thd->env = bindings;
  thd->expr = expr;
  Object res = evaluate(expr, thd);
  thd->expr = nullObj;
  thd->env = savedEnv;
  return res;
}

Object threadEnvLocate(Thread* thd, Object key) {
  Object env = threadGetEnv(thd);
  return listLocate(env, key);
}

Object threadGetEnv(Thread* thd) {
  return thd->env;
}

Object threadGetExn(Thread* thd) {
  return thd->exception;
}

void threadMark(Thread* thd) {
  objMark(thd->env);
  if (thd->expr.a != nullObj.a) {
    objMark(thd->expr);
  }
}

void threadMarkAll() {
  Thread* thd = runningThreads;
  while (thd) {
    threadMark(thd);
    thd = thd->next;
  }
}

Thread* threadNew() {
  Thread* thd;
  if (threadPool) {
    thd = threadPool;
    threadPool = thd->next;
  }
  else {
    thd = malloc(sizeof(Thread));
  }
  thd->env = EMPTY_LIST;
  thd->next = runningThreads;
  runningThreads = thd;
  return thd;
}

void threadSetEnv(Thread* thd, Object env) {
  thd->env = env;
}

void threadSetExpr(Thread* thd, Object expr) {
  thd->expr = expr;
}

void threadThrowException(Thread* thd, Object exn) {
  thd->exception.a = exn.a;
  longjmp(thd->jumpBuf, 1);
}
