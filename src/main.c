#include <stdio.h>

#include "globals.h"
#include "mem.h"
#include "object.h"
#include "repl.h"

void staticInit() {
  objStaticInit();
}

int main(int argc, char** argv) {
  (void)argc;  /* suppresses unused parameter warning */
  (void)argv;  /* suppresses unused parameter warning */
  memStart();
  staticInit();
  globalsSetup();
  repl();
  return 0;
}
