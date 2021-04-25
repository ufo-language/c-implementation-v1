#ifndef REPL_H
#define REPL_H

#include "object.h"

#define READ_BUF_SIZE 256

typedef struct Repl_struct {
  char inputBuffer[READ_BUF_SIZE];
  Object inputString;
  Object tokens;
  Object parseRes;
  Object value;
} ReplObj;

void repl(void);

#endif
