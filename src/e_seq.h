#ifndef E_SEQ_H
#define E_SEQ_H

#include "object.h"
#include "thread.h"

#define SEQ_EXPRS_OFS 0
#define SEQ_LAST_EXPR_OFS 1
#define SEQ_OBJ_SIZE 2

Object seqNew(Object exprList);

Object seqEval(Object seq, Thread* thd);
void seqFreeVars(Object seq, Object freeVarSet);
void seqMark(Object seq);
void seqShow(Object seq, FILE* stream);
void seqShowExprs(Object exprList, FILE* stream);

#endif
