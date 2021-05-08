#include <stdio.h>

#include "globals.h"
#include "mem.h"
#include "repl.h"

int main(int argc, char** argv) {
  (void)argc;  /* suppresses unused parameter warning */
  (void)argv;  /* suppresses unused parameter warning */
  memStart();
  globalsSetup();
  repl();
  return 0;
}
