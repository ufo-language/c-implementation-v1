#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "d_list.h"
#include "d_method.h"
#include "defines.h"
#include "delegate.h"
#include "e_app.h"
#include "e_binop.h"
#include "e_ident.h"
#include "e_quote.h"
#include "eval.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object methodApply(Object method, Object argList, Thread* thd) {
  Object receiver = {objGetData(method, METH_RECEIVER_OFS)};
  Object receiverVal = eval(receiver, thd);
  Object func = {objGetData(method, METH_FUNC_OFS)};
  char* receiverTypeName = ObjTypeNames[objGetType(receiverVal)];
  size_t nChars = strlen(receiverTypeName);
  char receiverTypeNameCopy[nChars + 1];
  strncpy(receiverTypeNameCopy, receiverTypeName, nChars + 1);
  receiverTypeNameCopy[0] = tolower(receiverTypeNameCopy[0]);
  Object namespaceIdent = identNew(receiverTypeNameCopy);
  Object argList1 = listNew(quoteNew(receiverVal), argList);
  Object colonExpr = binopNew(namespaceIdent, identNew(":"), func);
  Object app = appNew(colonExpr, argList1);
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
