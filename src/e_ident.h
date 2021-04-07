#ifndef E_IDENT_H
#define E_IDENT_H

#include "object.h"
#include "thread.h"

bool identEqual(Object ident, Object obj);
Object identEval(Object ident, Thread* thd);
Word identHash(Object ident);
Object identMatch(Object ident, Object other, Object bindingList);
Object identNew(char* name);
void identShow(Object ident, FILE* stream);

#endif
