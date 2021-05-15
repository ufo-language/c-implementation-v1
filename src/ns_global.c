#include "d_array.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_seq.h"
#include "delegate.h"
#include "e_ident.h"
#include "eval.h"
#include "globals.h"
#include "object.h"

Object oper_colon(Thread* thd, Object args);
Object oper_doubleDot(Thread* thd, Object args);

/*------------------------------------------------------------------*/
void global_defineAll(Object env) {
  hashPut(env, identNew(":"), primMacroNew(oper_colon));
  hashPut(env, identNew(".."), primNew(oper_doubleDot));
}

/*------------------------------------------------------------------*/
Object oper_colon(Thread* thd, Object args) {
  Object lhs = listGetFirst(args);
  Object lhsVal = eval(lhs, thd);
  Object rhs = listGetSecond(args);
  Object res = nullObj;
  switch (objGetType(lhsVal)) {
    case D_Hash:
      res = hashGet(lhsVal, rhs, thd);
      break;
    default: {
        Object exn = arrayN(3, lhs, lhsVal, rhs);
        threadThrowException(thd, "Error", "object {} ({}) can't be dereferenced by key {}", exn);
      }
  }
  return res;
}

/*------------------------------------------------------------------*/
Object oper_doubleDot(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  Object seq = seqNew(lhs, rhs, intNew(1));
  return seq;
}

