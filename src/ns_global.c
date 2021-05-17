#include "d_array.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_method.h"
#include "d_prim.h"
#include "d_seq.h"
#include "delegate.h"
#include "e_ident.h"
#include "eval.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

Object oper_colon(Thread* thd, Object args);
Object oper_dot(Thread* thd, Object args);
Object oper_doubleDot(Thread* thd, Object args);
Object oper_equal(Thread* thd, Object args);
Object oper_equalTo(Thread* thd, Object args);
Object oper_minus(Thread* thd, Object args);
Object oper_percent(Thread* thd, Object args);
Object oper_plus(Thread* thd, Object args);
Object oper_slash(Thread* thd, Object args);
Object oper_times(Thread* thd, Object args);

/*------------------------------------------------------------------*/
void global_defineAll(Object env) {
  hashPut(env, identNew(":"), primMacroNew(oper_colon));
  hashPut(env, identNew("."), primMacroNew(oper_dot));
  hashPut(env, identNew(".."), primNew(oper_doubleDot));
  hashPut(env, identNew("="), primNew(oper_equal));
  hashPut(env, identNew("=="), primNew(oper_equalTo));
  hashPut(env, identNew("+"), primNew(oper_plus));
  hashPut(env, identNew("-"), primNew(oper_minus));
  hashPut(env, identNew("*"), primNew(oper_times));
  hashPut(env, identNew("/"), primNew(oper_slash));
  hashPut(env, identNew("%"), primNew(oper_percent));
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
Object oper_dot(Thread* thd, Object args) {
  (void)thd;
  Object receiver = listGetFirst(args);
  Object func = listGetSecond(args);
  Object method = methodNew(receiver, func);
  return method;
}

/*------------------------------------------------------------------*/
Object oper_doubleDot(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  Object seq = seqNew(lhs, rhs, intNew(1), thd);
  return seq;
}

/*------------------------------------------------------------------*/
Object oper_equal(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  Object binding = bindingNew(lhs, rhs);
  return binding;
}

/*------------------------------------------------------------------*/
Object oper_equalTo(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  bool eq = objEquals(lhs, rhs);
  return eq ? TRUE : FALSE;
}

/*------------------------------------------------------------------*/
Object oper_minus(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  Object res = NOTHING;
  switch (objGetType(lhs)) {
    case D_Int:
      res = intBinop(lhs, rhs, intSubtract, thd);
      break;
    default: {
        Object exn = arrayN(2, lhs, rhs);
        threadThrowException(thd, "Error", "operator '-' can't be applied to a {} and a {}", exn);
      }
      break;
  }
  return res;
}

/*------------------------------------------------------------------*/
Object oper_percent(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  Object res = NOTHING;
  switch (objGetType(lhs)) {
    case D_Int:
      res = intBinop(lhs, rhs, intModulus, thd);
      break;
    case D_Seq:
      res = seqMod(lhs, rhs, thd);
      break;
    default: {
        Object exn = arrayN(2, lhs, rhs);
        threadThrowException(thd, "Error", "operator '%' can't be applied to a {} and a {}", exn);
      }
      break;
  }
  return res;
}

/*------------------------------------------------------------------*/
Object oper_plus(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  Object res = NOTHING;
  switch (objGetType(lhs)) {
    case D_Int:
      res = intBinop(lhs, rhs, intAdd, thd);
      break;
    default: {
        Object exn = arrayN(2, lhs, rhs);
        threadThrowException(thd, "Error", "operator '+' can't be applied to a {} and a {}", exn);
      }
      break;
  }
  return res;
}

/*------------------------------------------------------------------*/
Object oper_slash(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  Object res = NOTHING;
  switch (objGetType(lhs)) {
    case D_Int:
      res = intBinop(lhs, rhs, intDivide, thd);
      break;
    default: {
        Object exn = arrayN(2, lhs, rhs);
        threadThrowException(thd, "Error", "operator '/' can't be applied to a {} and a {}", exn);
      }
      break;
  }
  return res;
}

/*------------------------------------------------------------------*/
Object oper_times(Thread* thd, Object args) {
  (void)thd;
  Object lhs = listGetFirst(args);
  Object rhs = listGetSecond(args);
  Object res = NOTHING;
  switch (objGetType(lhs)) {
    case D_Int:
      res = intBinop(lhs, rhs, intMultiply, thd);
      break;
    default: {
        Object exn = arrayN(2, lhs, rhs);
        threadThrowException(thd, "Error", "operator '*' can't be applied to a {} and a {}", exn);
      }
      break;
  }
  return res;
}
