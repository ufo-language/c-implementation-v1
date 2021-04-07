#ifndef E_SYMBOL_H
#define E_SYMBOL_H

#include "object.h"
#include "thread.h"

bool symbolEqual(Object symbol, Object obj);
Word symbolHash(Object symbol);
Object symbolNew(char* name);
void symbolShow(Object symbol, FILE* stream);

#endif
