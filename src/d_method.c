#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "d_array.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_method.h"
#include "defines.h"
#include "delegate.h"
#include "e_app.h"
#include "e_binop.h"
#include "e_ident.h"
#include "e_quote.h"
#include "eval.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object methodApply(Object method, Object argList, Thread* thd) {
  Object receiver = {objGetData(method, METH_RECEIVER_OFS)};
  Object receiverVal = eval(receiver, thd);
  Object funcName = {objGetData(method, METH_FUNC_OFS)};
  char* receiverTypeName = ObjTypeNames[objGetType(receiverVal)];
  size_t nChars = strlen(receiverTypeName);
  char receiverTypeNameCopy[nChars + 1];
  strncpy(receiverTypeNameCopy, receiverTypeName, nChars + 1);
  receiverTypeNameCopy[0] = tolower(receiverTypeNameCopy[0]);
  Object receiverNamespaceIdent = identNew(receiverTypeNameCopy);
  Object applyable;
  for (int n=0; n<2; n++) {
    Object receiverNamespace = identLookup(receiverNamespaceIdent, thd);
     applyable = hashGet_unsafe(receiverNamespace, funcName, thd);
    if (applyable.a == nullObj.a) {
      if (n > 0) {
        Object exn = arrayN(2, funcName, receiverNamespaceIdent);
        threadThrowException(thd, "Error", "function {} not found in namespace {}", exn);
      }
      /* re-try using 'any' as the namespace */
      receiverNamespaceIdent = IDENT_ANY;
    }
  }
  Object argList1 = listNew(receiverVal, argList);
  Object app = appNew(applyable, argList1);
  Object res = appEval(app, thd);
  return res;
}

/*------------------------------------------------------------------*/
void methodMark(Object method) {
  Object func = {objGetData(method, METH_FUNC_OFS)};
  Object receiver = {objGetData(method, METH_RECEIVER_OFS)};
  objMark(func);
  objMark(receiver);
}

/*------------------------------------------------------------------*/
Object methodNew(Object receiver, Object func) {
  Object method = objAlloc(D_Method, METH_OBJ_SIZE);
  objSetData(method, METH_RECEIVER_OFS, receiver.a);
  objSetData(method, METH_FUNC_OFS, func.a);
  return method;
}

/*------------------------------------------------------------------*/
void methodShow(Object method, FILE* stream) {
  Object receiver = {objGetData(method, METH_RECEIVER_OFS)};
  Object func = {objGetData(method, METH_FUNC_OFS)};
  objShow(receiver, stream);
  fputc('.', stream);
  objShow(func, stream);
}
