#ifndef E_SYMBOL_H
#define E_SYMBOL_H

#include "object.h"
#include "thread.h"

bool symbolEquals(Object symbol, Object obj);
Word symbolHash(Object symbol);
bool symbolHasName(Object symbol, char* name);
Object symbolNew(char* name);
void symbolShow(Object symbol, FILE* stream);

#endif
