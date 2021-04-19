#ifndef THREAD_H
#define THREAD_H

#include <setjmp.h>

#include "object.h"

typedef struct Thread_struct {
  Object env;
  Object expr;
  jmp_buf jumpBuf;
  Object exception;
  struct Thread_struct* prev;
  struct Thread_struct* next;
} Thread;

Thread* threadNew();
void threadDelete(Thread* thd);

void threadEnvBind(Thread* thd, Object var, Object val);
Object threadEnvLocate(Thread* thd, Object key);
Object threadEval(Thread* thd, Object expr, Object bindings);
Object threadGetEnv(Thread* thd);
Object threadGetExn(Thread* thd);
void threadMark(Thread* thd);
void threadMarkAll();
void threadSetEnv(Thread* thd, Object env);
void threadSetExpr(Thread* thd, Object expr);
void threadThrowException(Thread* thd, Object exn);

#endif
