#ifndef E_BINOP_H
#define E_BINOP_H

#include "object.h"
#include "thread.h"

#define BINOP_LHS_OFS 0
#define BINOP_OPER_OFS 1
#define BINOP_RHS_OFS 2
#define BINOP_OBJ_SIZE 3

Object binopNew(Object lhs, Object oper, Object rhs);

Object binopEval(Object binop, Thread* thd);
void binopFreeVars(Object binop, Object freeVarSet);
void binopMark(Object binop);
void binopShow(Object binop, FILE* stream);

#endif
