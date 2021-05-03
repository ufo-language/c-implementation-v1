#include <stdio.h>
#include <string.h>

#include "delegate.h"
#include "gc.h"
#include "mem.h"
#include "repl.h"
#include "thread.h"

void colonCommandHelp() {
  puts("  :?    Shows this help message");
  puts("  :env  Shows current dynamic environment");
  puts("  :gc   Performs a garbage collection");
  puts("  :i    Shows last input");
  puts("  :mem  Shows memory values");
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
  else if (!strcmp(":gc", input)) {
    gcMarkSweep();
  }
  else if (!strcmp(":mem", input)) {
    printf("Free blocks: %d\n", memGetNBlocks());
    printf("Free words : %d\n", memGetNFreeWords());
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
