#include "delegate.h" // TODO for debugging
#include "eval.h"
#include "object.h"
#include "thread.h"
#include "trampoline.h"

Object trampEval(Object tramp, Thread* thd) {
  Object expr = trampGetExpr(tramp);
  Object env = trampGetEnv(tramp);
  Object res = threadEval(thd, expr, env);
  return res;
}

Object trampNew(Object expr, Object env) {
  Object tramp = objAlloc(S_Trampoline, TRAMP_OBJ_SIZE);
  objSetData(tramp, TRAMP_EXPR_OFS, expr.a);
  objSetData(tramp, TRAMP_ENV_OFS, env.a);
  return tramp;
}

Object trampGetExpr(Object tramp) {
  Object expr = {objGetData(tramp, TRAMP_EXPR_OFS)};
  return expr;
}

Object trampGetEnv(Object tramp) {
  Object env = {objGetData(tramp, TRAMP_ENV_OFS)};
  return env;
}

void trampMark(Object tramp) {
  Object expr = {objGetData(tramp, TRAMP_EXPR_OFS)};
  Object env = {objGetData(tramp, TRAMP_ENV_OFS)};
  objMark(expr);
  objMark(env);
}

void trampSet(Object tramp, Object expr, Object env) {
  objSetData(tramp, TRAMP_EXPR_OFS, expr.a);
  objSetData(tramp, TRAMP_ENV_OFS, env.a);  
}
