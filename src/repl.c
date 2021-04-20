#include <stdio.h>

#include "d_list.h"
#include "d_queue.h"
#include "d_string.h"
#include "delegate.h"
#include "eval.h"
#include "globals.h"
#include "lexer_obj.h"
#include "object.h"
#include "parser.h"
#include "repl.h"
#include "thread.h"

#define READ_BUF_SIZE 256

struct Repl_struct {
  Object inputString;
  Object tokens;
  Object parseRes;
  Object value;
};

void prompt() {
  printf("UFO> ");
}

int getLine(char* buffer, int len) {
  int i = 0;
  char c;
  while (i < len - 1) {
    c = getchar();
    if (c == '\n') {
      break;
    }
    buffer[i++] = c;
  }
  buffer[i] = '\0';
  if (c != '\n') {
    while ((c = getchar()) != '\n');
    printf("line too long, extra ignored\n");
  }
  return i;
}

Object read(Thread* thd, Object string) {
  Object tokenQ = lex(string);
  Object tokens = queueAsList(tokenQ);
  printf("read() lexer tokens = "); objShow(tokens, stdout); printf("\n");
  Object res = parseEntry(thd, tokens);
  return res;
}

void repl() {
  printf("^C to abort\n");
  Thread* thd = threadNew();
  struct Repl_struct repl;
  while (true) {
    prompt();
    char inputBuffer[READ_BUF_SIZE];
    int nChars = getLine(inputBuffer, READ_BUF_SIZE);
    if (nChars > 0) {
      printf("repl() input string = '%s'\n", inputBuffer);
      Object inputString = stringNew(inputBuffer);
//      Object parseRes = read(thd, inputString);
      Object tokenQ = lex(inputString);
      Object tokens = queueAsList(tokenQ);
      Object parseRes = parseEntry(thd, tokens);
      if (parseRes.a == nullObj.a) {
        printf("parse error\n");
      }
      else {
        Object obj = listGetFirst(parseRes);
        Object tokens = listGetRest(parseRes);
        if (listGetRest(tokens).a != EMPTY_LIST.a) {
          printf("repl() found too many tokens on line [probably because the parser is incomplete]\n");
          printf("repl() remaining tokens = "); objShow(tokens, stdout); printf("\n");
          /* TODO should I just call the parser on the remaining tokens? */
        }
        else {
          printf("repl() object = "); objShow(obj, stdout); printf(" : %s\n",ObjTypeNames[objGetType(obj)]);
          Object val = evaluate(obj, thd);
          if (val.a != nullObj.a) {
            printf("repl() val = "); objShow(val, stdout); printf(" : %s\n",ObjTypeNames[objGetType(val)]);
          }
        }
      }
    }
  }
}
