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
      printf("evaluate caught exception, payload = "); objShow(throwPayload, stdout); printf("\n");
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
  return objEval(obj, thd);
}

/*------------------------------------------------------------------*/
void evalThrowException(Object payload, Thread* thd) {
  /* TODO finish */
  fprintf(stderr, "%s is incomplete\n", __func__);
  (void)payload;  /* suppresses unused parameter warning */
  (void)thd;  /* suppresses unused parameter warning */
}
