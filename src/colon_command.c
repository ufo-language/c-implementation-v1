#include <stdio.h>
#include <string.h>

#include "delegate.h"
#include "repl.h"
#include "thread.h"

void colonCommandHelp() {
  puts("  :?    Shows this help message");
  puts("  :env  Shows current dynamic environment");
  puts("  :i    Shows last input");
  puts("  :q    Quits UFO");
}

void colonCommandShowInput(ReplObj* replObj) {
  printf("Input string: "); objShow(replObj->inputString, stdout); printf("\n");
  printf("Tokens:       "); objShow(replObj->tokens, stdout); printf("\n");
  printf("Parse result: "); objShow(replObj->parseRes, stdout);
  printf(" : %s\n", ObjTypeNames[objGetType(replObj->parseRes)]);
  printf("Value:        "); objShow(replObj->value, stdout);
  printf(" : %s\n", ObjTypeNames[objGetType(replObj->value)]);
}

bool colonCommand(Thread* thd, ReplObj* replObj) {
  (void)thd;
  char* input = replObj->inputBuffer;
  bool contin = true;
  if (!strcmp(":env", input)) {
    objShow(threadGetEnv(thd), stdout);
    printf("\n");
  }
  else if (!strcmp(":i", input)) {
    colonCommandShowInput(replObj);
  }
  else if (!strcmp(":q", input)) {
    contin = false;
  }
  else if (!strcmp(":?", input)) {
    colonCommandHelp();
  }
  else {
    printf("Colon command '%s' not understood\n", replObj->inputBuffer);
  }
  return contin;
}
