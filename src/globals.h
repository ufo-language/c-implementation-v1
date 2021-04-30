#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>

#include "object.h"

extern Object NOTHING;
extern Object EMPTY_LIST;
extern Object TRAMPOLINE;

void globalsMark();
void globalsSetup();

#endif
