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
Object oper_minus(Thread* thd, Object args);
Object oper_percent(Thread* thd, Object args);
Object oper_plus(Thread* thd, Object args);
Object oper_slash(Thread* thd, Object args);
Object oper_times(Thread* thd, Object args);

Object oper_lessThan(Thread* thd, Object args);
Object oper_lessThanOrEqual(Thread* thd, Object args);
Object oper_equalTo(Thread* thd, Object args);
Object oper_greaterThanOrEqual(Thread* thd, Object args);
Object oper_greaterThan(Thread* thd, Object args);
Object oper_notEqual(Thread* thd, Object args);

static Object param_AnyAny;

/*------------------------------------------------------------------*/
void global_defineAll(Object env, Thread* thd) {
  param_AnyAny = primBuildTypeList(2, D_Null, D_Null);
  hashPut(env, identNew(":"), primMacroNew(oper_colon), thd);
  hashPut(env, identNew("."), primMacroNew(oper_dot), thd);
  hashPut(env, identNew(".."), primNew(oper_doubleDot), thd);
  hashPut(env, identNew("="), primNew(oper_equal), thd);
  hashPut(env, identNew("+"), primNew(oper_plus), thd);
  hashPut(env, identNew("-"), primNew(oper_minus), thd);
  hashPut(env, identNew("*"), primNew(oper_times), thd);
  hashPut(env, identNew("/"), primNew(oper_slash), thd);
  hashPut(env, identNew("%"), primNew(oper_percent), thd);

  hashPut(env, identNew("<"), primNew(oper_lessThan), thd);
  hashPut(env, identNew("<="), primNew(oper_lessThanOrEqual), thd);
  hashPut(env, identNew("=="), primNew(oper_equalTo), thd);
  hashPut(env, identNew(">="), primNew(oper_greaterThanOrEqual), thd);
  hashPut(env, identNew(">"), primNew(oper_greaterThan), thd);
}

/*------------------------------------------------------------------*/
Object oper_colon(Thread* thd, Object args) {
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  Object lhsVal = eval(lhs, thd);
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
  Object receiver, func;
  Object* argAry[] = {&receiver, &func};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  Object method = methodNew(receiver, func);
  return method;
}

/*------------------------------------------------------------------*/
Object oper_doubleDot(Thread* thd, Object args) {
  Object from, to;
  Object* argAry[] = {&from, &to};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  Object seq = seqNew(from, to, intNew(1), thd);
  return seq;
}

/*------------------------------------------------------------------*/
Object oper_equal(Thread* thd, Object args) {
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  Object binding = bindingNew(lhs, rhs);
  return binding;
}

/*------------------------------------------------------------------*/
Object oper_equalTo(Thread* thd, Object args) {
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  bool eq = objEquals(lhs, rhs, thd);
  return eq ? TRUE : FALSE;
}

/*------------------------------------------------------------------*/
Object oper_greaterThan(Thread* thd, Object args) {
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  switch (objGetType(lhs)) {
    case D_Int:
      return intGreaterThan(lhs, rhs, thd) ? TRUE : FALSE;
    default: {
        Object exn = arrayN(4, lhs, ObjTypeNameSyms[objGetType(lhs)],
                               rhs, ObjTypeNameSyms[objGetType(rhs)]);
        threadThrowException(thd, "Error", "Unable to compare {} : {} > {} : {}", exn);
      }
  }
  return nullObj;
}

/*------------------------------------------------------------------*/
Object oper_greaterThanOrEqual(Thread* thd, Object args) {
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  switch (objGetType(lhs)) {
    case D_Int:
      return intGreaterThanOrEqual(lhs, rhs, thd) ? TRUE : FALSE;
    default: {
        Object exn = arrayN(4, lhs, ObjTypeNameSyms[objGetType(lhs)],
                               rhs, ObjTypeNameSyms[objGetType(rhs)]);
        threadThrowException(thd, "Error", "Unable to compare {} : {} >= {} : {}", exn);
      }
  }
  return nullObj;
}

/*------------------------------------------------------------------*/
Object oper_lessThan(Thread* thd, Object args) {
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  switch (objGetType(lhs)) {
    case D_Int:
      return intLessThan(lhs, rhs, thd) ? TRUE : FALSE;
    default: {
        Object exn = arrayN(4, lhs, ObjTypeNameSyms[objGetType(lhs)],
                               rhs, ObjTypeNameSyms[objGetType(rhs)]);
        threadThrowException(thd, "Error", "Unable to compare {} : {} < {} : {}", exn);
      }
  }
  return nullObj;
}

/*------------------------------------------------------------------*/
Object oper_lessThanOrEqual(Thread* thd, Object args) {
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
  switch (objGetType(lhs)) {
    case D_Int:
      return intLessThanOrEqual(lhs, rhs, thd) ? TRUE : FALSE;
    default: {
        Object exn = arrayN(4, lhs, ObjTypeNameSyms[objGetType(lhs)],
                               rhs, ObjTypeNameSyms[objGetType(rhs)]);
        threadThrowException(thd, "Error", "Unable to compare {} : {} <= {} : {}", exn);
      }
  }
  return nullObj;
}

/*------------------------------------------------------------------*/
Object oper_minus(Thread* thd, Object args) {
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
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
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
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
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
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
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
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
  Object lhs, rhs;
  Object* argAry[] = {&lhs, &rhs};
  primCheckArgs2(param_AnyAny, args, argAry, thd);
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
