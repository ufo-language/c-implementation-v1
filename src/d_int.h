#ifndef D_INT_H
#define D_INT_H

#include <stdio.h>

#include "defines.h"
#include "object.h"

Object intNew(int i);

bool intEquals(Object integer, Object other);
int intGet(Object integer);
int intHash(Object integer);
void intShow(Object integer, FILE* stream);

#endif
