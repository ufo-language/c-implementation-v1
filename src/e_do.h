#ifndef E_DO_H
#define E_DO_H

#include "object.h"
#include "thread.h"

#define DO_EXPRS_OFS 0
#define DO_LAST_EXPR_OFS 1
#define DO_OBJ_SIZE 2

Object doNew(Object exprList);

Object doEval(Object doExpr, Thread* thd);
void doFreeVars(Object doExpr, Object freeVarSet, Thread* thd);
void doMark(Object doExpr);
void doShow(Object doExpr, FILE* stream);
void doShowWith(char* start, Object doExpr, char* end, FILE* stream);

#endif
