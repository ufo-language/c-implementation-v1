#include <string.h>

#include "d_binding.h"
#include "d_list.h"
#include "d_string.h"
#include "e_ident.h"
#include "hash.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
bool identEquals(Object ident, Object obj) {
  /* treat ident as a string and compare */
  return stringEquals(ident, obj);
}

/*------------------------------------------------------------------*/
Object identEval(Object ident, Thread* thd) {
  Object env = threadGetEnv(thd);
  Object binding = listLocate(env, ident);
  if (binding.a == nullObj.a) {
    fprintf(stderr, "ERROR: unbound identiable: ");
    stringShow(ident, stderr);
    fprintf(stderr, "\n");
    return nullObj;
  }
  return bindingGetRhs(binding);
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
  Object ident = stringNew(str);
  objSetType(ident, E_Ident);
  return ident;
}

/*------------------------------------------------------------------*/
void identShow(Object ident, FILE* stream) {
  stringDisp(ident, stream);
}
