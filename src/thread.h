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

Thread* threadNew(void);
void threadDelete(Thread* thd);

void threadEnvBind(Thread* thd, Object var, Object val);
void threadEnvRebind(Thread* thd, Object var, Object val);
Object threadEnvLocate(Thread* thd, Object key);
Object threadEval(Thread* thd, Object expr, Object bindings);
Object threadGetEnv(Thread* thd);
Object threadGetExn(Thread* thd);
void threadMark(Thread* thd);
void threadMarkAll(void);
void threadSetEnv(Thread* thd, Object env);
void threadSetExpr(Thread* thd, Object expr);

void threadRestoreJump(Thread* thd, jmp_buf* jumpBuf);
int threadSetJump(Thread* thd, jmp_buf* jumpBuf);
void threadThrowException(Thread* thd, char* sym, char* message, Object obj);
void threadThrowExceptionObj(Thread* thd, Object exn);

#endif
