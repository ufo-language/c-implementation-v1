#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include "object.h"
#include "thread.h"

#define TRAMP_EXPR_OFS 0
#define TRAMP_ENV_OFS 1
#define TRAMP_OBJ_SIZE 2

Object trampEval(Object tramp, Thread* thd);
Object trampGetExpr(Object tramp);
Object trampGetEnv(Object tramp);
void trampMark(Object tramp);
void trampSet(Object tramp, Object expr, Object env);
void trampShow(Object tramp, FILE* stream);

#endif
