#ifndef E_SEQ_H
#define E_SEQ_H

#include "object.h"
#include "thread.h"

#define SEQ_EXPRS_OFS 0

Object seqNew(Object lst);

Object seqEval(Object seq, Thread* thd);
void seqFreeVars(Object seq, Object freeVarSet);
void seqMark(Object seq);
void seqShow(Object seq, FILE* stream);

#endif
