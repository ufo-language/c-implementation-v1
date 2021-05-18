#include <stdio.h>

#include "d_binding.h"
#include "delegate.h"
#include "eval.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object bindingNew(Object lhs, Object rhs) {
  Object binding = objAlloc(D_Binding, BND_OBJ_SIZE);
  objSetData(binding, BND_LHS_OFS, lhs.a);
  objSetData(binding, BND_RHS_OFS, rhs.a);
  return binding;
}

/*------------------------------------------------------------------*/
bool bindingEquals(Object binding, Object other, Thread* thd) {
  Object lhs1 = bindingGetLhs(binding);
  Object lhs2 = bindingGetLhs(other);
  if (!objEquals(lhs1, lhs2, thd)) {
    return false;
  }
  Object rhs1 = bindingGetRhs(binding);
  Object rhs2 = bindingGetRhs(other);
  return objEquals(rhs1, rhs2, thd);
}

/*------------------------------------------------------------------*/
Object bindingEval(Object binding, Thread* thd) {
  Object newLhs = eval(bindingGetLhs(binding), thd);
  Object newRhs = eval(bindingGetRhs(binding), thd);
  return bindingNew(newLhs, newRhs);
}

/*------------------------------------------------------------------*/
void bindingFreeVars(Object binding, Object freeVarSet, Thread* thd) {
  objFreeVars(bindingGetLhs(binding), freeVarSet, thd);
  objFreeVars(bindingGetRhs(binding), freeVarSet, thd);
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
void bindingMark(Object binding) {
  objMark(bindingGetLhs(binding));
  objMark(bindingGetRhs(binding));
}

/*------------------------------------------------------------------*/
Object bindingMatch(Object binding, Object other, Object bindingList, Thread* thd) {
  Object lhs1 = bindingGetLhs(binding);
  Object lhs2 = bindingGetLhs(other);
  bindingList = objMatch(lhs1, lhs2, bindingList, thd);
  if (bindingList.a == nullObj.a) {
    return nullObj;
  }
  Object rhs1 = bindingGetRhs(binding);
  Object rhs2 = bindingGetRhs(other);
  return objMatch(rhs1, rhs2, bindingList, thd);
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
