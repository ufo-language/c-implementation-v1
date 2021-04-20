#ifndef COLON_COMMAND_H
#define COLON_COMMAND_H

#include "repl.h"
#include "thread.h"

/* returns true to continue, false to quit */
bool colonCommand(Thread* thd, ReplObj* replObj);

#endif
