#include <stdio.h> 

#include "d_hash.h"
#include "d_int.h"
#include "d_list.h"
#include "d_prim.h"
#include "d_string.h"
#include "d_stringbuffer.h"
#include "delegate.h"
#include "e_ident.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object stringBuffer_count(Thread* thd, Object args);
Object stringBuffer_new(Thread* thd, Object args);
Object stringBuffer_readChar(Thread* thd, Object args);
Object stringBuffer_toString(Thread* thd, Object args);
Object stringBuffer_write(Thread* thd, Object args);

static Object param_Sb;
static Object param_SbString;

/*------------------------------------------------------------------*/
void stringBuffer_defineAll(Object env, Thread* thd) {
  char* nsName = "stringBuffer";
  param_Sb = primBuildTypeList(1, D_StringBuffer);
  param_SbString = primBuildTypeList(2, D_StringBuffer, D_String);
  Object ns = hashNew();
  nsAddPrim(ns, "count", stringBuffer_count, thd);
  nsAddPrim(ns, "new", stringBuffer_new, thd);
  nsAddPrim(ns, "readChar", stringBuffer_readChar, thd);
  nsAddPrim(ns, "toString", stringBuffer_toString, thd);
  nsAddPrim(ns, "write", stringBuffer_write, thd);
  hashPut(env, identNew(nsName), ns, thd);
}

/*------------------------------------------------------------------*/
Object stringBuffer_count(Thread* thd, Object args) {
  Object sb;
  Object* argAry[] = {&sb};
  primCheckArgs(param_Sb, args, argAry, thd);
  return intNew(stringBufferCount(sb));
}

/*------------------------------------------------------------------*/
Object stringBuffer_new(Thread* thd, Object args) {
  primCheckArgs(EMPTY_LIST, args, NULL, thd);
  Object sb = stringBufferNew();
  return sb;
}

/*------------------------------------------------------------------*/
Object stringBuffer_readChar(Thread* thd, Object args) {
  Object sb;
  Object* argAry[] = {&sb};
  primCheckArgs(param_Sb, args, argAry, thd);
  Object string = stringBufferReadChar(sb, thd);
  return string;
}

/*------------------------------------------------------------------*/
Object stringBuffer_toString(Thread* thd, Object args) {
  Object sb;
  Object* argAry[] = {&sb};
  primCheckArgs(param_Sb, args, argAry, thd);
  Object string = stringBufferToString(sb);
  return string;
}

/*------------------------------------------------------------------*/
Object stringBuffer_write(Thread* thd, Object args) {
  Object sb, string;
  Object* argAry[] = {&sb, &string};
  primCheckArgs(param_SbString, args, argAry, thd);
  stringBufferWrite(sb, string);
  return sb;
}
