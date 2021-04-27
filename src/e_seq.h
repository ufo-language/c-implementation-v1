#ifndef E_SEQ_H
#define E_SEQ_H

#include "object.h"
#include "thread.h"

#define SEQ_EXPRS_OFS 0
#define SEQ_LAST_EXPR_OFS 1

Object seqNew(Object lst);

Object seqEval(Object seq, Thread* thd);
void seqFreeVars(Object seq, Object freeVarSet);
void seqShow(Object seq, FILE* stream);
void seqShowExprs(Object seq, FILE* stream);

#endif
