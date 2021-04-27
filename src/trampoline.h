#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#define TRAMP_EXPR_OFS 0
#define TRAMP_ENV_OFS 1

Object trampNew(Object expr, Object env);

Object trampEval(Object tramp, Thread* thd);
Object trampGetExpr(Object tramp);
Object trampGetEnv(Object tramp);

#endif
