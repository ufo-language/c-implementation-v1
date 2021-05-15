#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "d_string.h"
#include "d_stringbuffer.h"
#include "delegate.h"
#include "gc.h"
#include "mem.h"
#include "repl.h"
#include "thread.h"

void colonCommandHelp() {
  puts("  :?         Shows this help message");
  puts("  :e         Enter multiple lines, empty line to terminate");
  puts("  :env       Shows current dynamic environment");
  puts("  :gc        Performs a garbage collection");
  puts("  :i         Shows last input");
  puts("  :mem       Shows memory values");
  puts("  :mr <addr> Shows the value of memory at addr");
  puts("  :q         Quits UFO");
  puts("  :size      Shows system word sizes");
}

void colonCommandReadMultipleLines(ReplObj* repl) {
  getLines(repl->lineBuffer);
  repl->inputString = stringBufferToString(repl->lineBuffer);
}

void colonCommandShowInput(ReplObj* replObj) {
  printf("Input string: "); objShow(replObj->inputString, stdout); printf("\n");
  printf("Tokens:       "); objShow(replObj->tokens, stdout); printf("\n");
  printf("Parse result: "); objShow(replObj->parseRes, stdout);
  printf(" : %s\n", ObjTypeNames[objGetType(replObj->parseRes)]);
  printf("Value:        "); objShow(replObj->value, stdout);
  printf(" : %s\n", ObjTypeNames[objGetType(replObj->value)]);
}

void colonCommandShowSizes() {
  printf("System stats:\n");
  printf("Size of char:  %ld bits\n", sizeof(char) * 8);
  printf("Size of int:   %ld bits\n", sizeof(int) * 8);
  printf("Size of long:  %ld bits\n", sizeof(long) * 8);
  printf("Size of void*: %ld bits\n", sizeof(void*) * 8);
  printf("Size of uint:  %ld bits\n", sizeof(uint) * 8);
  printf("  where uint = unsigned short int\n");
}

bool startsWith(char* prefix, char* str) {
  return strncmp(prefix, str, strlen(prefix)) == 0;
}

bool colonCommand(Thread* thd, ReplObj* replObj) {
  (void)thd;
  char* input = replObj->inputBuffer;
  bool res = false;
  if (!strcmp(":e", input)) {
    colonCommandReadMultipleLines(replObj);
    res = true;
  }
  else if (!strcmp(":env", input)) {
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
  else if (startsWith(":mr", input)) {
    Word addr = atoi(input + 4);
    Word val = vmemGet(addr);
    printf("value @%d = %d\n", addr, val);
  }
  else if (!strcmp(":q", input)) {
    replObj->contin = false;
  }
  else if (!strcmp(":size", input)) {
    colonCommandShowSizes();
  }
  else if (!strcmp(":?", input)) {
    colonCommandHelp();
  }
  else {
    printf("Colon command '%s' not understood\n", replObj->inputBuffer);
  }
  return res;
}
