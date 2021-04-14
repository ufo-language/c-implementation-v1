#include <stdio.h>

#include "d_binding.h"
#include "delegate.h"
#include "eval.h"
#include "gc.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object bindingNew(Object lhs, Object rhs) {
  Object binding = objAlloc(D_Binding, 2);
  objSetData(binding, BND_LHS_OFS, lhs.a);
  objSetData(binding, BND_RHS_OFS, rhs.a);
  return binding;
}

/*------------------------------------------------------------------*/
bool bindingEquals(Object binding, Object other) {
  Object lhs1 = bindingGetLhs(binding);
  Object lhs2 = bindingGetLhs(other);
  if (!objEquals(lhs1, lhs2)) {
    return false;
  }
  Object rhs1 = bindingGetRhs(binding);
  Object rhs2 = bindingGetRhs(other);
  return objEquals(rhs1, rhs2);
}

/*------------------------------------------------------------------*/
Object bindingEval(Object binding, Thread* thd) {
  Object newLhs = eval(bindingGetLhs(binding), thd);
  Object newRhs = eval(bindingGetRhs(binding), thd);
  return bindingNew(newLhs, newRhs);
}

/*------------------------------------------------------------------*/
void bindingFreeVars(Object binding, Object freeVarSet) {
  objFreeVars(bindingGetLhs(binding), freeVarSet);
  objFreeVars(bindingGetRhs(binding), freeVarSet);
}

/*------------------------------------------------------------------*/
Object bindingGetLhs(Object binding) {
  Object obj = {objGetData(binding, BND_LHS_OFS)};
  return obj;
}

/*------------------------------------------------------------------*/
Object bindingGetRhs(Object binding) {
  Object obj = {objGetData(binding, BND_RHS_OFS)};
  return obj;
}

/*------------------------------------------------------------------*/
Object bindingMatch(Object binding, Object other, Object bindingList) {
  Object lhs1 = bindingGetLhs(binding);
  Object lhs2 = bindingGetLhs(other);
  bindingList = objMatch(lhs1, lhs2, bindingList);
  if (bindingList.a == nullObj.a) {
    return nullObj;
  }
  Object rhs1 = bindingGetRhs(binding);
  Object rhs2 = bindingGetRhs(other);
  return objMatch(rhs1, rhs2, bindingList);
}

/*------------------------------------------------------------------*/
void bindingSetRhs(Object binding, Object obj) {
  objSetData(binding, BND_RHS_OFS, obj.a);
}

/*------------------------------------------------------------------*/
void bindingShow(Object binding, FILE* stream) {
  objShow(bindingGetLhs(binding), stream);
  fputc('=', stream);
  objShow(bindingGetRhs(binding), stream);
}
