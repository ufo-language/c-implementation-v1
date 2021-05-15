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
Object stringBuffer_write(Thread* thd, Object args);

static Object param_Sb;
static Object param_SbString;

/*------------------------------------------------------------------*/
void stringBuffer_defineAll(Object env) {
  char* nsName = "stringBuffer";
  param_Sb = primBuildTypeList(1, D_StringBuffer);
  param_SbString = primBuildTypeList(2, D_StringBuffer, D_String);
  Object ns = hashNew();
  nsAddPrim(ns, "count", stringBuffer_count);
  nsAddPrim(ns, "new", stringBuffer_new);
  nsAddPrim(ns, "readChar", stringBuffer_readChar);
  nsAddPrim(ns, "write", stringBuffer_write);
  hashPut(env, identNew(nsName), ns);
}

/*------------------------------------------------------------------*/
Object stringBuffer_count(Thread* thd, Object args) {
  primCheckArgs(param_Sb, args, thd);
  Object sb = listGetFirst(args);
  return intNew(stringBufferCount(sb));
}

/*------------------------------------------------------------------*/
Object stringBuffer_new(Thread* thd, Object args) {
  primCheckArgs(EMPTY_LIST, args, thd);
  Object sb = stringBufferNew();
  return sb;
}

/*------------------------------------------------------------------*/
Object stringBuffer_readChar(Thread* thd, Object args) {
  primCheckArgs(param_Sb, args, thd);
  Object sb = listGetFirst(args);
  Object string = stringBufferReadChar(sb, thd);
  return string;
}

/*------------------------------------------------------------------*/
Object stringBuffer_write(Thread* thd, Object args) {
  primCheckArgs(param_SbString, args, thd);
  Object sb = listGetFirst(args);
  Object string = listGetSecond(args);
  stringBufferWrite(sb, string);
  return sb;
}
