#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>

#include "object.h"
#include "thread.h"

extern Object NOTHING;
extern Object EMPTY_LIST;
extern Object TRUE;
extern Object FALSE;
extern Object GLOBALS;
extern Object SUPER_GLOBALS;
extern Object IDENT_ANY;

void globalsMark(void);
void globalsSetup(void);
void namespacesSetup(Thread* thd);
Object registerGlobal(Object obj);

#endif
