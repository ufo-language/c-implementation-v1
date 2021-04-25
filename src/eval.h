#ifndef EVAL_H
#define EVAL_H

#include "object.h"
#include "thread.h"

/* This is the entry point. It does a recursive evaluation of `obj`
   and after that completes it gc-commits all the new objects. */
Object evaluate(Object obj, Thread* thd);

/* This is the function that should be called by objects that are
   evaluating their children. New/uncommitted objects will not be
   swept by the garbage collector. */
Object eval(Object obj, Thread* thd);

#endif
