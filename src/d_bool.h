#ifndef D_BOOL_H
#define D_BOOL_H

#include <stdio.h>

#include "defines.h"
#include "object.h"

#define BOOL_TRUE_STRING "true"
#define BOOL_FALSE_STRING "false"

Object boolNew(bool b);

bool boolEqual(Object bln, Object other);
bool boolGet(Object bln);
void boolShow(Object bln, FILE* stream);

#endif
