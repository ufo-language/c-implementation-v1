#include <stdio.h>

#include "d_list.h"
#include "delegate.h"
#include "e_quote.h"
#include "eval.h"
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
void quoteFreeVars(Object quote, Object freeVarSet, Thread* thd) {
  Object expr = {objGetData(quote, QUOTE_EXPR_OFS)};
  objFreeVars(expr, freeVarSet, thd);
}

/*------------------------------------------------------------------*/
void quoteMark(Object quote) {
  Object expr = {objGetData(quote, QUOTE_EXPR_OFS)};
  objMark(expr);
}

/*------------------------------------------------------------------*/
Object quoteNew(Object lst) {
  Object quote = objAlloc(E_Quote, QUOTE_OBJ_SIZE);
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
