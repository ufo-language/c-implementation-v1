#ifndef D_SEQ_H
#define D_SEQ_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

#define SEQ_FROM_OFS 0
#define SEQ_TO_OFS 1
#define SEQ_STEP_OFS 2
#define SEQ_OBJ_SIZE 3

Object seqNew(Object from, Object to, Object step, Thread* thd);

bool seqEquals(Object seq, Object other);
Object seqEval(Object seq, Thread* thd);
Object seqGetFrom(Object seq);
Object seqGetTo(Object seq);
Object seqGetStep(Object seq);
void seqMark(Object seq);
Object seqMod(Object seq, Object step, Thread* thd);
void seqShow(Object seq, FILE* stream);

#endif
