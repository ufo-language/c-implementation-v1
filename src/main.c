#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "mem.h"

/* This is a parody of a main file. Look in test/ for actual code that runs */

#if 0
void getLine(char* buffer, int len) {
  int i = 0;
  char c;
  while (i < len) {
    c = getchar();
    if (c == '\n') {
      buffer[i] = '\0';
      return;
    }
    buffer[i++] = c;
  }
  while ((c = getchar()) != '\n');
  printf("line too long, extra ignored\n");
}
#endif

void showStats() {
  printf("System stats:\n");
  printf("Size of char:  %ld bits\n", sizeof(char) * 8);
  printf("Size of int:   %ld bits\n", sizeof(int) * 8);
  printf("Size of long:  %ld bits\n", sizeof(long) * 8);
  printf("Size of void*: %ld bits\n", sizeof(void*) * 8);
  printf("Size of uint:  %ld bits\n", sizeof(uint) * 8);
}

int main(int argc, char** argv) {
  printf("main is quite incomplete\n");
  showStats();
  return 0;
}
