#ifndef E_QUOTE_H
#define E_QUOTE_H

#include "object.h"
#include "thread.h"

#define QUOTE_EXPR_OFS 0
#define QUOTE_OBJ_SIZE 1

Object quoteNew(Object lst);

Object quoteEval(Object quote, Thread* thd);
void quoteFreeVars(Object quote, Object freeVarSet);
void quoteMark(Object quote);
void quoteShow(Object quote, FILE* stream);

#endif
