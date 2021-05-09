#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>

#include "object.h"

extern Object NOTHING;
extern Object EMPTY_LIST;
extern Object STANDARD_ENV;
extern Object TRUE;
extern Object FALSE;

void globalsMark();
void globalsSetup();

#endif
