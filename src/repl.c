#include <stdio.h>

#include "colon_command.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_queue.h"
#include "d_string.h"
#include "d_stringbuffer.h"
#include "delegate.h"
#include "e_ident.h"
#include "eval.h"
#include "globals.h"
#include "lexer_obj.h"
#include "object.h"
#include "parser.h"
#include "repl.h"
#include "thread.h"

ReplObj theRepl;

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
void getLines(Object stringBuffer) {
  Object newline = stringNew("\n");
  while (true) {
    int nChars = getLine(theRepl.inputBuffer, READ_BUF_SIZE);
    if (nChars == 0) {
      break;
    }
    Object string = stringNew(theRepl.inputBuffer);
    stringBufferWrite(stringBuffer, string);
    stringBufferWrite(stringBuffer, newline);
  }
}

/*------------------------------------------------------------------*/
void repl(void) {
  replInit(&theRepl);
  intro();
  Object it = identNew("it");
  hashPut(SUPER_GLOBALS, it, NOTHING);
  Thread* thd = threadNew();
  while (theRepl.contin) {
    int jumpRes = setjmp(thd->jumpBuf);
    if (jumpRes != 0) {
      Object exn = threadGetExn(thd);
      fprintf(stderr, "REPL caught exception\n");
      objShow(exn, stderr);
      fprintf(stderr, "\n");
      continue;
    }
    prompt();
    int nChars = getLine(theRepl.inputBuffer, READ_BUF_SIZE);
    if (nChars > 0) {
      if (theRepl.inputBuffer[0] == ':') {
        if (!colonCommand(thd, &theRepl)) {
          /* returns false for all commands except ':e' multi-line input */
          continue;
        }
      }
      else {
        theRepl.inputString = stringNew(theRepl.inputBuffer);
      }
      Object tokenQ = lex(thd, theRepl.inputString);
      theRepl.tokens = queueAsList(tokenQ);
      theRepl.parseRes = parseEntry(thd, theRepl.tokens);
      if (theRepl.parseRes.a != nullObj.a) {
        Object obj = listGetFirst(theRepl.parseRes);
        Object tokens = listGetRest(theRepl.parseRes);
        if (listGetRest(tokens).a != EMPTY_LIST.a) {
          fprintf(stderr, "REPL ERROR: too many tokens on line (probably because the parser is incomplete)\n");
          fprintf(stderr, "  remaining tokens = "); objShow(tokens, stderr); fprintf(stderr, "\n");
          /* TODO should I just call the parser on the remaining tokens? */
        }
        else {
          theRepl.parseRes = obj;
          theRepl.value = nullObj;
          Object val = evaluate(obj, thd);
          if (val.a != nullObj.a) {
            theRepl.value = val;
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

/*------------------------------------------------------------------*/
void replInit(ReplObj* repl) {
  repl->lineBuffer = stringBufferNew();
  repl->inputString = stringNew("");
  repl->tokens = EMPTY_LIST;
  repl-> parseRes = NOTHING;
  repl->value = NOTHING;
  repl->contin = true;
}

/*------------------------------------------------------------------*/
void replMark() {
  objMark(theRepl.lineBuffer);
  objMark(theRepl.inputString);
  objMark(theRepl.tokens);
  objMark(theRepl.parseRes);
  objMark(theRepl.value);
}
