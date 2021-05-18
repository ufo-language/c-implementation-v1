#include <stdio.h>

#include "d_array.h"
#include "d_binding.h"
#include "d_list.h"
#include "d_set.h"
#include "delegate.h"
#include "e_letrec.h"
#include "eval.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object letRecEval(Object letRec, Thread* thd) {
  Object bindings = {objGetData(letRec, LETREC_BINDINGS_OFS)};
  /* collect all lhs variables from bindings */
  Object varSet = setNew(); 
  while (!listIsEmpty(bindings)) {
    Object binding = listGetFirst(bindings);
    Object lhs = bindingGetLhs(binding);
    objFreeVars(lhs, varSet, thd);
    bindings = listGetRest(bindings);
  }
  /* bind all lhs variables to nothing */
  Object varAry = setToArray(varSet);
  Word nVars = arrayCount(varAry);
  for (Word n=0; n<nVars; n++) {
    Object var = arrayGet_unsafe(varAry, n);
    threadEnvBind(thd, var, NOTHING);
  }
  /* evaluate all bindings, rebind lhs vars as necessary */
  bindings.a = objGetData(letRec, LETREC_BINDINGS_OFS);
  while (!listIsEmpty(bindings)) {
    Object binding = listGetFirst(bindings);
    Object lhs = bindingGetLhs(binding);
    Object rhsExpr = bindingGetRhs(binding);
    Object rhsVal = eval(rhsExpr, thd);
    Object evaluatedBindings = objMatch(lhs, rhsVal, EMPTY_LIST, thd);
    /* rebind variables in the envionment */
    while (!listIsEmpty(evaluatedBindings)) {
      Object evaluatedBinding = listGetFirst(evaluatedBindings);
      Object ident = bindingGetLhs(evaluatedBinding);
      Object value = bindingGetRhs(evaluatedBinding);
      threadEnvRebind(thd, ident, value);
      evaluatedBindings = listGetRest(evaluatedBindings);
    }
    bindings = listGetRest(bindings);
  }
  return NOTHING;
}

/*------------------------------------------------------------------*/
void letRecFreeVars(Object letRec, Object freeVarSet, Thread* thd) {
  Object lhsVars = setNew();
  Object bindings = {objGetData(letRec, LETREC_BINDINGS_OFS)};
  while (!listIsEmpty(bindings)) {
    Object binding = listGetFirst(bindings);
    Object lhs = bindingGetLhs(binding);
    Object rhs = bindingGetRhs(binding);
    /* separate the vars on the left from the vars on the right */
    objFreeVars(lhs, lhsVars, thd);
    objFreeVars(rhs, freeVarSet, thd);
    bindings = listGetRest(bindings);
  }
  /* remove each var in the lhs set from the rhs set */
  setRemoveSet(freeVarSet, lhsVars, thd);
}

/*------------------------------------------------------------------*/
void letRecMark(Object letRec) { 
  Object bindings = {objGetData(letRec, LETREC_BINDINGS_OFS)};
  objMark(bindings);
}

/*------------------------------------------------------------------*/
Object letRecNew(Object bindings) {
  Object letRec = objAlloc(E_LetRec, LETREC_OBJ_SIZE);
  objSetData(letRec, LETREC_BINDINGS_OFS, bindings.a);
  return letRec;
}

/*------------------------------------------------------------------*/
void letRecShow(Object letRec, FILE* stream) {
  Object bindings = {objGetData(letRec, LETREC_BINDINGS_OFS)};
  listShowWith(bindings, "letrec ", ", ", "", stream);
}
