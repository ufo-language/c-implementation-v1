#include <stdio.h>

#include "d_binding.h"
#include "d_list.h"
#include "d_set.h"
#include "delegate.h"
#include "e_let.h"
#include "eval.h"
#include "gc.h"
#include "globals.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object letEval(Object let, Thread* thd) {
  Object bindings = {objGetData(let, LET_BINDINGS_OFS)};
  while (!listIsEmpty(bindings)) {
    Object binding = listGetFirst(bindings);
    Object var = bindingGetLhs(binding);
    Object valExpr = bindingGetRhs(binding);
    Object bindingVal = eval(valExpr, thd);
    threadEnvBind(thd, var, bindingVal);
    bindings = listGetRest(bindings);
  }
  return NOTHING;
}

/*------------------------------------------------------------------*/
void letFreeVars(Object let, Object freeVarSet) {
  Object lhsVars = setNew();
  Object bindings = {objGetData(let, LET_BINDINGS_OFS)};
  while (!listIsEmpty(bindings)) {
    Object binding = listGetFirst(bindings);
    Object lhs = bindingGetLhs(binding);
    Object rhs = bindingGetRhs(binding);
    /* separate the vars on the left from the vars on the right */
    objFreeVars(lhs, lhsVars);
    objFreeVars(rhs, freeVarSet);
    bindings = listGetRest(bindings);
  }
  /* remove each var in the lhs set from the rhs set */
  setRemoveSet(freeVarSet, lhsVars);
}

/*------------------------------------------------------------------*/
void letMark(Object let) {
  Object bindings = {objGetData(let, LET_BINDINGS_OFS)};
  objMark(bindings);
}

/*------------------------------------------------------------------*/
Object letNew(Object bindings) {
  Object let = objAlloc(E_Let, LET_OBJ_SIZE);
  objSetData(let, LET_BINDINGS_OFS, bindings.a);
  return let;
}

/*------------------------------------------------------------------*/
void letShow(Object let, FILE* stream) {
  Object bindings = {objGetData(let, LET_BINDINGS_OFS)};
  listShowWith(bindings, "let ", ", ", "", stream);
}
