#include <string.h>

#include "d_array.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_string.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "hash.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
/* Destructive modification of a binding. */
void identAssign(Object ident, Object val, Thread* thd) {
  Object binding = threadEnvLocate(thd, ident);
  if (binding.a == nullObj.a) {
    threadThrowException(thd, "EvaluatorError", "unbound identifier", ident);
  }
  bindingSetRhs(binding, val);
}

/*------------------------------------------------------------------*/
bool identEquals(Object ident, Object obj) {
  /* it's already determined that obj is an E_Ident */
  /* treat ident as a string and compare */
  return stringEquals(ident, obj);
}

/*------------------------------------------------------------------*/
Object identEval(Object ident, Thread* thd) {
  Object binding = threadEnvLocate(thd, ident);
  if (binding.a != nullObj.a) {
    return bindingGetRhs(binding);
  }
  Object val = hashGet_unsafe(GLOBALS, ident, thd);
  if (val.a != nullObj.a) {
    return val;
  }
  val = hashGet_unsafe(SUPER_GLOBALS, ident, thd);
  if (val.a == nullObj.a) {
    threadThrowException(thd, "EvaluatorError", "unbound identifier", ident);
  }
  return val;
}

/*------------------------------------------------------------------*/
Word identHash(Object ident) {
  return stringHash_aux(ident) ^ hashPrimes(objGetType(ident));
}

/*------------------------------------------------------------------*/
Object identMatch(Object ident, Object other, Object bindingList) {
  if (ident.a == other.a) {
    return bindingList;
  }
  ObjType objTypeOther = objGetType(other);
  if (objTypeOther == E_Ident) {
    return nullObj;
  }
  Object binding = bindingNew(ident, other);
  return listNew(binding, bindingList);
}

/*------------------------------------------------------------------*/
Object identNew(char* str) {
  /* an identifier is a string with its ObjType set to E_Ident */
  Object ident = stringNew(str);
  objSetType(ident, E_Ident);
  return ident;
}

/*------------------------------------------------------------------*/
void identShow(Object ident, FILE* stream) {
  stringDisp(ident, stream);
}
