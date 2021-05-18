#include <stdio.h>

#include "d_binding.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_string.h"
#include "delegate.h"
#include "e_ident.h"
#include "namespace.h"
#include "object.h"

Object string_substring(Thread* thd, Object args);

static Object param_StringIntInt;

/*------------------------------------------------------------------*/
void string_defineAll(Object env, Thread* thd) {
  char* nsName = "string";
  param_StringIntInt = primBuildTypeList(3, D_String, D_Int, D_Int);
  Object ns = hashNew();
  nsAddPrim(ns, "substring", string_substring, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
Object string_substring(Thread* thd, Object args) {
  Object string, start, end;
  Object* argAry[] = {&string, &start, &end};
  primCheckArgs2(param_StringIntInt, args, argAry, thd);
  Object substr = stringSubstring(string, (Word)intGet(start), (Word)intGet(end));
  return substr;
}
