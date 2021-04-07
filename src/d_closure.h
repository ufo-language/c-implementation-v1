#ifndef D_CLOSURE_H
#define D_CLOSURE_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

/* be sure that these match the offsets in e_abstr.h */
#define CLO_PARAMS_OFS 0
#define CLO_BODY_OFS 1
#define CLO_NEXT_OFS 2

#define CLO_LEXENV_OFS 3

Object closureNew(Object abstr, Object lexEnv);

Object closureApply(Object closure, Object argList, Thread* thd);
void closureMark(Object closure);
void closureShow(Object closure, FILE* stream);

#endif
