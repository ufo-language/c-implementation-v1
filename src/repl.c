#include <stdio.h>

#include "colon_command.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_queue.h"
#include "d_string.h"
#include "delegate.h"
#include "e_ident.h"
#include "eval.h"
#include "globals.h"
#include "lexer_obj.h"
#include "object.h"
#include "parser.h"
#include "repl.h"
#include "thread.h"

/*------------------------------------------------------------------*/
void intro(void) {
  puts("▌▐ ▛▘▛▜ UFO version 4-rc-1");
  puts("▙▟ ▛ ▙▟ http://github.com/ufo-language");
  puts("type :? for help");
  puts("");
}

/*------------------------------------------------------------------*/
void prompt(void) {
  printf("UFO> ");
}

/*------------------------------------------------------------------*/
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

/*------------------------------------------------------------------*/
void repl(void) {
  intro();
  Object it = identNew("it");
  hashPut(SUPER_GLOBALS, it, NOTHING);
  Thread* thd = threadNew();
  ReplObj repl;
  bool contin = true;
  while (contin) {
    int jumpRes = setjmp(thd->jumpBuf);
    if (jumpRes != 0) {
      Object exn = threadGetExn(thd);
      fprintf(stderr, "REPL caught exception\n");
      objShow(exn, stderr);
      fprintf(stderr, "\n");
      continue;
    }
    prompt();
    int nChars = getLine(repl.inputBuffer, READ_BUF_SIZE);
    if (nChars > 0) {
      if (repl.inputBuffer[0] == ':') {
        contin = colonCommand(thd, &repl);
        continue;
      }
      repl.inputString = stringNew(repl.inputBuffer);
      Object tokenQ = lex(thd, repl.inputString);
      repl.tokens = queueAsList(tokenQ);
      repl.parseRes = parseEntry(thd, repl.tokens);
      if (repl.parseRes.a != nullObj.a) {
        Object obj = listGetFirst(repl.parseRes);
        Object tokens = listGetRest(repl.parseRes);
        if (listGetRest(tokens).a != EMPTY_LIST.a) {
          fprintf(stderr, "REPL ERROR: too many tokens on line (probably because the parser is incomplete)\n");
          fprintf(stderr, "  remaining tokens = "); objShow(tokens, stderr); fprintf(stderr, "\n");
          /* TODO should I just call the parser on the remaining tokens? */
        }
        else {
          repl.parseRes = obj;
          repl.value = nullObj;
          Object val = evaluate(obj, thd);
          if (val.a != nullObj.a) {
            repl.value = val;
            if (val.a != NOTHING.a) {
              objShow(val, stdout);
              printf(" :: %s\n", ObjTypeNames[objGetType(val)]);
              hashPut(SUPER_GLOBALS, it, val);
            }
          }
        }
      }
      else {
        fprintf(stderr, "REPL: parse error\n");
      }
    }
  }
}
