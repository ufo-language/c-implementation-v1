#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include "object.h"
#include "thread.h"

#define TRAMP_EXPR_OFS 0
#define TRAMP_ENV_OFS 1

Object trampEval(Object tramp, Thread* thd);
Object trampGetExpr(Object tramp);
Object trampGetEnv(Object tramp);
void trampSet(Object tramp, Object expr, Object env);

#endif
