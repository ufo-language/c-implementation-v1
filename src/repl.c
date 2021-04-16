#include <stdio.h>

#include "delegate.h"
#include "object.h"
#include "repl.h"

#define READ_BUF_SIZE 256

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

Object read() {
  char buffer[READ_BUF_SIZE];
  getLine(buffer, READ_BUF_SIZE);
  printf("read got buffer '%s'\n", buffer);
  return nullObj;
}

void repl() {
  Object obj = read();
  printf("repl got object "); objShow(obj, stdout); printf("\n");
}
