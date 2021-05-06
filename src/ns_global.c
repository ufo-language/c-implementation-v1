#include "d_array.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_prim.h"
#include "delegate.h"
#include "e_ident.h"
#include "eval.h"
#include "globals.h"
#include "object.h"

Object oper_colon(Thread* thd, Object args);

Object global_defineAll(Object env) {
  Object ident = identNew(":");
  Object prim = primMacroNew(oper_colon);
  Object binding = bindingNew(ident, prim);
  env = listNew(binding, env);
  return env;
}

Object oper_colon(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object lhsVal = eval(lhs, thd);
  Object rhs = listGetSecond(args);
  Object res = nullObj;
  switch (objGetType(lhsVal)) {
    case D_Hash:
      res = hashGet(lhsVal, rhs, thd);
      break;
    default: {
        Object exn = arrayNew(3);
        arraySet_unsafe(exn, 0, lhs);
        arraySet_unsafe(exn, 1, lhsVal);
        arraySet_unsafe(exn, 2, rhs);
        threadThrowException(thd, "Error", "object {} ({}) can't be dereferenced by key {}", exn);
      }
  }
  return res;
}
