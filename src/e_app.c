#include "d_array.h"
#include "d_closure.h"
#include "d_list.h"
#include "d_method.h"
#include "d_prim.h"
#include "d_string.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_app.h"
#include "e_ident.h"
#include "eval.h"
#include "object.h"

/*------------------------------------------------------------------*/
Object appEval(Object app, Thread* thd) {
  Object abstr = {objGetData(app, APP_ABSTR_OFS)};
  Object args = {objGetData(app, APP_ARGS_OFS)};
  Object abstrVal = eval(abstr, thd);
  Object argsVal = nullObj;
  Object res = nullObj;
  switch (objGetType(abstrVal)) {
    case D_Closure:
      argsVal = eval(args, thd);
      res = closureApply(abstrVal, argsVal, thd);
      break;
    case D_Method:
      argsVal = eval(args, thd);
      res = methodApply(abstrVal, argsVal, thd);
      break;
    case D_Prim:
      argsVal = eval(args, thd);
      res = primApply(abstrVal, argsVal, thd);
      break;
    case D_PrimMacro:
      res = primApply(abstrVal, args, thd);
      break;
    default:
      threadThrowException(thd, "Error", "object is not an abstraction", abstrVal);
  }
  return res;
}

/*------------------------------------------------------------------*/
void appFreeVars(Object app, Object freeVarSet, Thread* thd) {
  Object abstr = {objGetData(app, APP_ABSTR_OFS)};
  Object args = {objGetData(app, APP_ARGS_OFS)};
  objFreeVars(abstr, freeVarSet, thd);
  objFreeVars(args, freeVarSet, thd);
}

/*------------------------------------------------------------------*/
void appMark(Object app) {
  Object abstr = {objGetData(app, APP_ABSTR_OFS)};
  Object args = {objGetData(app, APP_ARGS_OFS)};
  objMark(abstr);
  objMark(args);
}

/*------------------------------------------------------------------*/
Object appNew(Object abstr, Object args) {
  Object app = objAlloc(E_App, APP_OBJ_SIZE);
  objSetData(app, APP_ABSTR_OFS, abstr.a);
  objSetData(app, APP_ARGS_OFS, args.a);
  return app;
}

/*------------------------------------------------------------------*/
void appShow(Object app, FILE* stream) {
  Object abstr = {objGetData(app, APP_ABSTR_OFS)};
  Object args = {objGetData(app, APP_ARGS_OFS)};
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
