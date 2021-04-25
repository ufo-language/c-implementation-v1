#include <stdio.h>

#include "d_list.h"
#include "delegate.h"
#include "e_quote.h"
#include "eval.h"
#include "gc.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object quoteEval(Object quote, Thread* thd) {
  (void)thd;
  Object expr = {objGetData(quote, QUOTE_EXPR_OFS)};
  return expr;
}

/*------------------------------------------------------------------*/
void quoteFreeVars(Object quote, Object freeVarSet) {
  Object expr = {objGetData(quote, QUOTE_EXPR_OFS)};
  objFreeVars(expr, freeVarSet);
}

/*------------------------------------------------------------------*/
Object quoteNew(Object lst) {
  Object quote = objAlloc(E_Quote, 1);
  objSetData(quote, QUOTE_EXPR_OFS, lst.a);
  return quote;
}

/*------------------------------------------------------------------*/
void quoteShow(Object quote, FILE* stream) {
  fputc('\'', stream);
  Object expr = {objGetData(quote, QUOTE_EXPR_OFS)};
  objShow(expr, stream);
  fputc('\'', stream);
}