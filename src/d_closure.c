#include "d_array.h"
#include "d_binding.h"
#include "d_closure.h"
#include "d_list.h"
#include "d_set.h"
#include "delegate.h"
#include "e_abstr.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

void abstrFreeVars_rule(Object rule, Object freeVarSet);
void abstrShow_aux(Object abstr, char* prefix, FILE* stream);

/* Returns a minimal lexical environment for an abstraction rule */
Object _close(Object closure, Object env);

/*------------------------------------------------------------------*/
Object closureApply(Object closure, Object argList, Thread* thd) {
  Object origClosure = closure;
  /* iterate over the rules until one matches */
  while (closure.a != nullObj.a) {
    Object paramList = {objGetData(closure, CLO_PARAMS_OFS)};
    Object body = {objGetData(closure, CLO_BODY_OFS)};
    Object lexEnv = {objGetData(closure, CLO_LEXENV_OFS)};
    Object bindings = objMatch(paramList, argList, lexEnv);
    if (bindings.a != nullObj.a) {
      Object res = threadEval(thd, body, bindings);
      return res;
    }
    closure.a = objGetData(closure, CLO_NEXT_OFS);
  }
  Object obj = listNew(origClosure, argList);
  threadThrowException(thd, "ArgumentMismatch", "arguments do not match any rule in function", obj);
  return nullObj;
}

/*------------------------------------------------------------------*/
void closureMark(Object closure) {
  Object paramList = {objGetData(closure, CLO_PARAMS_OFS)};
  Object body = {objGetData(closure, CLO_BODY_OFS)};
  Object next = {objGetData(closure, CLO_NEXT_OFS)};
  Object lexEnv = {objGetData(closure, CLO_LEXENV_OFS)};
  objMark(paramList);
  objMark(body);
  objMark(lexEnv);
  if (next.a != nullObj.a) {
    objMark(next);
  }
}

/*------------------------------------------------------------------*/
Object closureNew(Object abstr, Object env) {
  Object firstRule = nullObj;
  Object prevRule = nullObj;
  while (abstr.a != nullObj.a) {
    Object rule = objAlloc(D_Closure, 4);
    if (firstRule.a == nullObj.a) {
      firstRule = rule;
    }
    if (prevRule.a != nullObj.a) {
      objSetData(prevRule, CLO_NEXT_OFS, rule.a);
    }
    objSetData(rule, CLO_PARAMS_OFS, objGetData(abstr, ABSTR_PARAMS_OFS));
    objSetData(rule, CLO_BODY_OFS, objGetData(abstr, ABSTR_BODY_OFS));
    Object lexEnv = _close(rule, env);
    objSetData(rule, CLO_LEXENV_OFS, lexEnv.a);
    prevRule = rule;
    abstr.a = objGetData(abstr, CLO_NEXT_OFS);
  }
  return firstRule;
}

/*------------------------------------------------------------------*/
void closureShow(Object closure, FILE* stream) {
  abstrShow_aux(closure, "fun*", stream);
}

/*------------------------------------------------------------------*/
Object _close(Object rule, Object env) {
  Object freeVarSet = setNew();
  abstrFreeVars_rule(rule, freeVarSet);
  Object freeVarAry = setToArray(freeVarSet);
  Word nElems = arrayCount(freeVarAry);
  Object lexEnv = EMPTY_LIST;
  for (Word n=0; n<nElems; n++) {
    Object ident = arrayGet(freeVarAry, n);
    Object binding = listLocate(env, ident);
    if (binding.a != nullObj.a) {
      lexEnv = listNew(binding, lexEnv);
    }
  }
  return lexEnv;
}
