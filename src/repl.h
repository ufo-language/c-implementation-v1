#ifndef REPL_H
#define REPL_H

#include "object.h"
#include "thread.h"

#define READ_BUF_SIZE 256

typedef struct Repl_struct {
  Object lineBuffer;
  char inputBuffer[READ_BUF_SIZE];
  Object inputString;
  Object tokens;
  Object parseRes;
  Object value;
  bool contin;
} ReplObj;

int getLine(char* buffer, int len);
void getLines(Object stringBuffer);
void repl(Thread* thd);
void replInit(ReplObj* repl);
void replMark(void);

#endif
