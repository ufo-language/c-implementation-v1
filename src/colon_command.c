#include <stdio.h>
#include <string.h>

#include "delegate.h"
#include "repl.h"
#include "thread.h"

void colonCommand(Thread* thd, ReplObj* replObj) {
  printf("colonCommand called\n");
  if (!strcmp(":i", replObj->inputBuffer)) {
    printf("Last input\n");
    printf("Input string: "); objShow(replObj->inputString, stdout); printf("\n");
    printf("Tokens:       "); objShow(replObj->tokens, stdout); printf("\n");
    printf("Parse result: "); objShow(replObj->parseRes, stdout); printf("\n");
    printf("Value:        "); objShow(replObj->value, stdout); printf("\n");
  }
  else {
    printf("Colon command '%s' not understood\n", replObj->inputBuffer);
  }
}
