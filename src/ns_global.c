#include "d_array.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_prim.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "object.h"

Object oper_colon(Thread* thd, Object args);

Object global_defineAll(Object env) {
  Object ident = identNew(":");
  Object prim = primNew(oper_colon);
  Object binding = bindingNew(ident, prim);
  env = listNew(binding, env);
  return env;
}

Object oper_colon(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  Object res = nullObj;
  switch (objGetType(lhs)) {
    case D_Hash:
      res = hashGet(lhs, rhs, thd);
      break;
    default: {
        Object exn = arrayNew(2);
        arraySet_unsafe(exn, 0, lhs);
        arraySet_unsafe(exn, 1, rhs);
        threadThrowException(thd, "Error", "object can't be dereferenced by key", exn);
      }
  }
  return res;
}
