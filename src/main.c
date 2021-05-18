#include <stdio.h>

#include "globals.h"
#include "mem.h"
#include "object.h"
#include "repl.h"

extern int main_argc;
extern char** main_argv;

void staticInit() {
  objStaticInit();
}

int main(int argc, char** argv) {
  main_argc = argc;
  main_argv = argv;
  memStart();
  staticInit();
  Thread* thd = threadNew();
  globalsSetup(thd);
  repl(thd);
  return 0;
}
