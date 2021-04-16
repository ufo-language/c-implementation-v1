#include <stdio.h>

#include "globals.h"
#include "mem.h"
#include "repl.h"

#if 0
void showStats() {
  printf("System stats:\n");
  printf("Size of char:  %ld bits\n", sizeof(char) * 8);
  printf("Size of int:   %ld bits\n", sizeof(int) * 8);
  printf("Size of long:  %ld bits\n", sizeof(long) * 8);
  printf("Size of void*: %ld bits\n", sizeof(void*) * 8);
  printf("Size of uint:  %ld bits\n", sizeof(uint) * 8);
}
#endif

int main(int argc, char** argv) {
  memStart();
  globalsSetup();
  repl();
  return 0;
}
