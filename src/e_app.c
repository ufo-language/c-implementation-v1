#include <assert.h>

#include "d_array.h"
#include "d_closure.h"
#include "d_list.h"
#include "d_string.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_app.h"
#include "e_ident.h"
#include "eval.h"
#include "object.h"

Object appEval(Object app, Thread* thd) {
  Object abstr = {objGetData(app, APP_ABSTR_OFS)};
  Object args = {objGetData(app, APP_ARGS_OFS)};
  Object abstrVal = eval(abstr, thd);
  Object argsVal = eval(args, thd);
  if (objGetType(abstrVal) != D_Closure) {
    threadThrowException(thd, "Error", "object is not an abstraction", abstrVal);
  }
  Object res = closureApply(abstrVal, argsVal, thd);
  return res;
}

void appFreeVars(Object app, Object freeVarSet) {
  Object abstr = {objGetData(app, APP_ABSTR_OFS)};
  Object args = {objGetData(app, APP_ARGS_OFS)};
  objFreeVars(abstr, freeVarSet);
  objFreeVars(args, freeVarSet);
}

void appMark(Object app) {
  Object abstr = {objGetData(app, APP_ABSTR_OFS)};
  Object args = {objGetData(app, APP_ARGS_OFS)};
  objMark(abstr);
  objMark(args);
}

Object appNew(Object abstr, Object args) {
  Object app = objAlloc(E_App, APP_OBJ_SIZE);
  objSetData(app, APP_ABSTR_OFS, abstr.a);
  objSetData(app, APP_ARGS_OFS, args.a);
  return app;
}

void appShow(Object app, FILE* stream) {
  Object abstr = {objGetData(app, APP_ABSTR_OFS)};
  Object args = {objGetData(app, APP_ARGS_OFS)};
  assert(objGetType(args) == D_List);
  if (objGetType(abstr) == E_Ident) {
    identShow(abstr, stream);
  }
  else {
    fputc('(', stream);
    objShow(abstr, stream);
    fputc(')', stream);
  }
  listShowWith(args, "(", ", ", ")", stream);
}
