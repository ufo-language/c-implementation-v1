#include <setjmp.h>

#include "delegate.h"
#include "e_throw.h"
#include "eval.h"
#include "gc.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object evaluate(Object obj, Thread* thd) {
  int jumpRes = setjmp(thd->jumpBuf);
  switch (jumpRes) {
    case 0: {  /* standard eval */
        Object res = eval(obj, thd);
        gcCommit();
        return res;
      }
    case 1:  /* caught exception */
      fprintf(stderr, "Evaluator caught exception:\n");
      objShow(thd->exception, stderr);
      fprintf(stderr, "\n");
      return nullObj;
    case 2:  /* thread context switch */
      fprintf(stderr, "evaluate thread switch is not implemented\n");
      return nullObj;
    default:
      fprintf(stderr, "evaluate switch does not handle case %d\n", jumpRes);
      return nullObj;      
  }
}

/*------------------------------------------------------------------*/
Object eval(Object obj, Thread* thd) {
  Object res = objEval(obj, thd);
  return res;
}
